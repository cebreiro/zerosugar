#include "navi_visualizer.h"

#include <DebugDraw.h>
#include <boost/scope/scope_fail.hpp>
#include <recastnavigation/DebugDraw.h>
#include <recastnavigation/DetourDebugDraw.h>
#include <recastnavigation/Recast.h>
#include <recastnavigation/RecastDebugDraw.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <gl/GLU.h>

#include "zerosugar/shared/collision/shape/obb.h"
#include "zerosugar/xr/navigation/navi_data.h"
#include "zerosugar/xr/navigation/RecastDemo/InputGeom.h"
#include "zerosugar/xr/navigation/RecastDemo/imgui.h"
#include "zerosugar/xr/navigation/RecastDemo/imguiRenderGL.h"
#include "zerosugar/xr/navigation/RecastDemo/NavMeshTesterTool.h"
#include "zerosugar/xr/navigation/RecastDemo/Sample.h"
#include "zerosugar/xr/navigation/RecastDemo/TestCase.h"

namespace
{
    enum DrawMode
    {
        DRAWMODE_NAVMESH,
        DRAWMODE_NAVMESH_TRANS,
        DRAWMODE_NAVMESH_BVTREE,
        DRAWMODE_NAVMESH_NODES,
        DRAWMODE_NAVMESH_PORTALS,
        DRAWMODE_NAVMESH_INVIS,
        DRAWMODE_MESH,
        MAX_DRAWMODE
    };
}

namespace zerosugar::xr::navi
{
    Visualizer::Visualizer(Strand& strand, Data& data)
        : _strand(strand)
        , _data(data)
        , _dd(std::make_unique<SampleDebugDraw>())
        , _testTool(std::make_unique<NavMeshTesterTool>())
    {
    }

    Visualizer::~Visualizer()
    {
        Shutdown();

        _renderThread.join();
    }

    auto Visualizer::Run(Promise<std::pair<bool, std::string>>& initPromise) -> Future<void>
    {
        _geom = std::make_unique<InputGeom>();
        if (const bool success = _geom->load(nullptr, GetObjectFilePath()); !success)
        {
            assert(false);

            co_return;
        }

        _testTool->init(this);

        Promise<void> promise;
        Future<void> future = promise.GetFuture();

        _renderThread = std::thread([this, &initPromise, p = std::move(promise)]() mutable
            {
                this->RenderThreadMain(initPromise);

                p.Set();
            });

        co_await future;

        co_return;
    }

    void Visualizer::Shutdown()
    {
        _shutdown.store(true);
    }

    auto Visualizer::AddAgent(int64_t id, vis::Agent agent) -> Future<bool>
    {
        if (!ExecutionContext::IsEqualTo(_strand))
        {
            co_await _strand;
        }

        assert(ExecutionContext::IsEqualTo(_strand));

        co_return _agents.try_emplace(id, std::move(agent)).second;
    }

    auto Visualizer::RemoveAgent(int64_t id) -> Future<bool>
    {
        if (!ExecutionContext::IsEqualTo(_strand))
        {
            co_await _strand;
        }

        assert(ExecutionContext::IsEqualTo(_strand));

        co_return _agents.erase(id);
    }

    auto Visualizer::UpdateAgentPosition(int64_t id, Eigen::Vector3d position) -> Future<bool>
    {
        if (!ExecutionContext::IsEqualTo(_strand))
        {
            co_await _strand;
        }

        assert(ExecutionContext::IsEqualTo(_strand));

        const auto iter = _agents.find(id);
        if (iter == _agents.end())
        {
            co_return false;
        }

        iter->second.position = position;

        co_return true;
    }

    auto Visualizer::UpdateAgentPositionAndYaw(int64_t id, Eigen::Vector3d position, float yaw) -> Future<bool>
    {
        if (!ExecutionContext::IsEqualTo(_strand))
        {
            co_await _strand;
        }

        assert(ExecutionContext::IsEqualTo(_strand));

        const auto iter = _agents.find(id);
        if (iter == _agents.end())
        {
            co_return false;
        }

        iter->second.position = position;
        iter->second.yaw = yaw;

        co_return true;
    }

    auto Visualizer::UpdateAgentMovement(int64_t id, Eigen::Vector3d startPos, vis::Agent::Movement movement) -> Future<bool>
    {
        if (!ExecutionContext::IsEqualTo(_strand))
        {
            co_await _strand;
        }

        assert(ExecutionContext::IsEqualTo(_strand));

        const auto iter = _agents.find(id);
        if (iter == _agents.end())
        {
            co_return false;
        }

        iter->second.position = startPos;
        iter->second.movement = movement;

        co_return true;
    }

    auto Visualizer::Draw(vis::Object object, std::chrono::milliseconds milli) -> Future<void>
    {
        if (!ExecutionContext::IsEqualTo(_strand))
        {
            co_await _strand;
        }

        assert(ExecutionContext::IsEqualTo(_strand));

        (void)_objects.emplace(std::chrono::system_clock::now() + milli, std::move(object));

        co_return;
    }

    void Visualizer::Tick()
    {
        if (_agents.empty() && _objects.empty())
        {
            return;
        }

        auto now = std::chrono::system_clock::now();

        for (vis::Agent& agent : _agents | std::views::values)
        {
            DrawAgentYawArrow(agent.position, agent.radius, agent.yaw, agent.drawColor);
            DrawAgentCylinder(agent.position, agent.radius, agent.drawColor);

            if (agent.movement)
            {
                vis::Agent::Movement& movement = *agent.movement;

                const double duration = std::chrono::duration_cast<std::chrono::duration<double>>(now - movement.startTimePoint).count();
                const double t = std::clamp(duration / movement.duration, 0.0, 1.0);

                const Eigen::Vector3d currentPos = (1.0 - t) * agent.position + t * movement.destPosition;

                DrawAgentCylinder(currentPos, agent.radius, movement.drawColor);

                if (t >= 1.0)
                {
                    agent.position = movement.destPosition;

                    agent.movement.reset();
                }
            }
        }

        auto outDateIter = _objects.end();

        for (auto iter = _objects.begin(); iter != _objects.end(); ++iter)
        {
            if (const auto removeTimePoint = iter->first; removeTimePoint < now)
            {
                outDateIter = iter;
            }

            std::visit([this, drawColor = iter->second.drawColor]<typename T>(const T& shape)
            {
                if constexpr (std::is_same_v<T, vis::Cylinder>)
                {
                    this->DrawCylinder(Vector(shape.min), Vector(shape.max), drawColor);
                }
                else if constexpr (std::is_same_v<T, vis::OBB>)
                {
                    this->DrawOBB(shape, drawColor);
                }
                else if constexpr (std::is_same_v<T, vis::Circle>)
                {
                    this->DrawCircle(shape, drawColor);
                }
                else if constexpr (std::is_same_v<T, vis::Arrow>)
                {
                    this->DrawArrow(shape, drawColor);
                }
                else if constexpr (std::is_same_v<T, vis::Lines>)
                {
                    this->DrawLines(shape, drawColor);
                }
                else
                {
                    static_assert(!sizeof(T), "not implemented");
                }
                
            }, iter->second.shape);
        }

        if (outDateIter != _objects.end())
        {
            _objects.erase(_objects.begin(), outDateIter);
        }
    }

    void Visualizer::DrawAgentCylinder(const Eigen::Vector3d& pos, double radius, vis::DrawColor drawColor)
    {
        constexpr float height = 2.5f;

        const Vector naviPos(pos);
        const float naviRadius = Scalar(radius).Get();

        const Vector min(naviPos.GetX() - naviRadius, naviPos.GetY(), naviPos.GetZ() - naviRadius);
        const Vector max(naviPos.GetX() + naviRadius, naviPos.GetY() + height, naviPos.GetZ() + naviRadius);

        DrawCylinder(min, max, drawColor);
    }

    void Visualizer::DrawAgentYawArrow(const Eigen::Vector3d& pos, double radius, double yaw, vis::DrawColor drawColor)
    {
        constexpr double length = 150.0;

        const Eigen::AngleAxisd axis(yaw * std::numbers::pi / 180.0, Eigen::Vector3d::UnitZ());
        const auto rotation = axis.toRotationMatrix();

        const vis::Arrow arrow{
            .startPos = pos + rotation * Eigen::Vector3d(radius, 0.0, 0.0),
            .endPos = pos + rotation * Eigen::Vector3d(radius + length, 0.0, 0.0),
        };

        DrawArrow(arrow, drawColor, 0.f, 0.2f, 3.f);
    }

    void Visualizer::DrawCylinder(const Vector& min, const Vector& max, vis::DrawColor drawColor)
    {
        duDebugDrawCylinder(_dd.get(),
            min.GetX(), min.GetY(), min.GetZ(),
            max.GetX(), max.GetY(), max.GetZ(),
            ToInt(drawColor));
    }

    void Visualizer::DrawOBB(const vis::OBB& obb, vis::DrawColor drawColor)
    {
        const auto computeVertices = [](const vis::OBB& obb)->std::array<Vector, 8>
        {
            std::array<Vector, 8> result;

            const std::array extents{
                obb.halfSize,
                Eigen::Vector3d(-obb.halfSize.x(),  obb.halfSize.y(),  obb.halfSize.z()),
                Eigen::Vector3d(obb.halfSize.x(), -obb.halfSize.y(),  obb.halfSize.z()),
                Eigen::Vector3d(obb.halfSize.x(),  obb.halfSize.y(), -obb.halfSize.z()),
                Eigen::Vector3d(-obb.halfSize.x(), -obb.halfSize.y(),  obb.halfSize.z()),
                Eigen::Vector3d(-obb.halfSize.x(),  obb.halfSize.y(), -obb.halfSize.z()),
                Eigen::Vector3d(obb.halfSize.x(), -obb.halfSize.y(), -obb.halfSize.z()),
                Eigen::Vector3d(-obb.halfSize.x(), -obb.halfSize.y(), -obb.halfSize.z())
            };

            for (int64_t i = 0; i < std::ssize(extents); ++i)
            {
                const Eigen::Vector3d value = obb.center + obb.rotation * extents[i];

                result[i] = Vector(value);
            }

            return result;
        };

        const auto& vertices = computeVertices(obb);

        GLfloat currentColor[4];
        glGetFloatv(GL_CURRENT_COLOR, currentColor);

        const std::array<uint8_t, 4> color = ToArray(drawColor);
        glColor4ub(color[0], color[1], color[2], color[3]);

        glBegin(GL_LINES);

        glVertex3fv(vertices[0].GetData()); glVertex3fv(vertices[1].GetData());
        glVertex3fv(vertices[1].GetData()); glVertex3fv(vertices[4].GetData());
        glVertex3fv(vertices[4].GetData()); glVertex3fv(vertices[2].GetData());
        glVertex3fv(vertices[2].GetData()); glVertex3fv(vertices[0].GetData());

        glVertex3fv(vertices[3].GetData()); glVertex3fv(vertices[5].GetData());
        glVertex3fv(vertices[5].GetData()); glVertex3fv(vertices[7].GetData());
        glVertex3fv(vertices[7].GetData()); glVertex3fv(vertices[6].GetData());
        glVertex3fv(vertices[6].GetData()); glVertex3fv(vertices[3].GetData());

        glVertex3fv(vertices[0].GetData()); glVertex3fv(vertices[3].GetData());
        glVertex3fv(vertices[1].GetData()); glVertex3fv(vertices[5].GetData());
        glVertex3fv(vertices[4].GetData()); glVertex3fv(vertices[7].GetData());
        glVertex3fv(vertices[2].GetData()); glVertex3fv(vertices[6].GetData());

        glEnd();

        glColor4fv(currentColor);
    }

    void Visualizer::DrawCircle(const vis::Circle& circle, vis::DrawColor drawColor, float lineWidth)
    {
        const auto& center = Vector(circle.center);

        duDebugDrawCircle(_dd.get(),
            center.GetX(), center.GetY(), center.GetZ(),
            Scalar(circle.radius).Get(),
            ToInt(drawColor),
            lineWidth);
    }

    void Visualizer::DrawArrow(const vis::Arrow& arrow, vis::DrawColor drawColor, float as0, float as1, float lineWidth)
    {
        const auto& startPos = Vector(arrow.startPos);
        const auto& endPos = Vector(arrow.endPos);

        duDebugDrawArrow(_dd.get(),
            startPos.GetX(), startPos.GetY(), startPos.GetZ(),
            endPos.GetX(), endPos.GetY(), endPos.GetZ(),
            as0, as1,
            ToInt(drawColor),
            lineWidth);
    }

    void Visualizer::DrawLines(const vis::Lines& lines, vis::DrawColor drawColor)
    {
        if (std::ssize(lines.positions) < 2)
        {
            return;
        }

        GLfloat currentColor[4];
        glGetFloatv(GL_CURRENT_COLOR, currentColor);

        const std::array<uint8_t, 4> color = ToArray(drawColor);
        glColor4ub(color[0], color[1], color[2], color[3]);

        glBegin(GL_LINE_STRIP);

        for (const Eigen::Vector3d& point : lines.positions)
        {
            Vector current(point);
            glVertex3fv(current.GetData());
        }

        glEnd();

        glColor4fv(currentColor);
    }

    void Visualizer::handleTools()
    {
        imguiIndent();

        _testTool->handleMenu();

        imguiUnindent();
    }

    void Visualizer::handleDebugMode()
    {
        // Check which modes are valid.
        bool valid[MAX_DRAWMODE];
        for (int i = 0; i < MAX_DRAWMODE; ++i)
            valid[i] = false;

        if (_geom)
        {
            valid[DRAWMODE_NAVMESH] = true;
            valid[DRAWMODE_NAVMESH_TRANS] = true;
            valid[DRAWMODE_NAVMESH_BVTREE] = true;
            valid[DRAWMODE_NAVMESH_NODES] = true;
            valid[DRAWMODE_NAVMESH_PORTALS] = true;
            valid[DRAWMODE_NAVMESH_INVIS] = true;
            valid[DRAWMODE_MESH] = true;
        }

        int unavail = 0;
        for (int i = 0; i < MAX_DRAWMODE; ++i)
            if (!valid[i]) unavail++;

        if (unavail == MAX_DRAWMODE)
            return;

        imguiLabel("Draw");
        if (imguiCheck("Input Mesh", _drawMode == DRAWMODE_MESH, valid[DRAWMODE_MESH]))
            _drawMode = DRAWMODE_MESH;
        if (imguiCheck("Navmesh", _drawMode == DRAWMODE_NAVMESH, valid[DRAWMODE_NAVMESH]))
            _drawMode = DRAWMODE_NAVMESH;
        if (imguiCheck("Navmesh Invis", _drawMode == DRAWMODE_NAVMESH_INVIS, valid[DRAWMODE_NAVMESH_INVIS]))
            _drawMode = DRAWMODE_NAVMESH_INVIS;
        if (imguiCheck("Navmesh Trans", _drawMode == DRAWMODE_NAVMESH_TRANS, valid[DRAWMODE_NAVMESH_TRANS]))
            _drawMode = DRAWMODE_NAVMESH_TRANS;
        if (imguiCheck("Navmesh BVTree", _drawMode == DRAWMODE_NAVMESH_BVTREE, valid[DRAWMODE_NAVMESH_BVTREE]))
            _drawMode = DRAWMODE_NAVMESH_BVTREE;
        if (imguiCheck("Navmesh Nodes", _drawMode == DRAWMODE_NAVMESH_NODES, valid[DRAWMODE_NAVMESH_NODES]))
            _drawMode = DRAWMODE_NAVMESH_NODES;
        if (imguiCheck("Navmesh Portals", _drawMode == DRAWMODE_NAVMESH_PORTALS, valid[DRAWMODE_NAVMESH_PORTALS]))
            _drawMode = DRAWMODE_NAVMESH_PORTALS;
    }

    void Visualizer::handleClick(const float* s, const float* p, bool shift)
    {
        _testTool->handleClick(s, p, shift);
    }

    void Visualizer::handleToggle()
    {
        _testTool->handleToggle();
    }

    void Visualizer::handleStep()
    {
        _testTool->handleStep();
    }

    void Visualizer::handleRender()
    {
        constexpr float cellSize = 0.3f;
        constexpr float agentMaxSlope = 45.f;

        const float texScale = 1.0f / (cellSize * 10.0f);

        // Draw mesh
        if (_drawMode != DRAWMODE_NAVMESH_TRANS)
        {
            // Draw mesh
            duDebugDrawTriMeshSlope(_dd.get(), _geom->getMesh()->getVerts(), _geom->getMesh()->getVertCount(),
                _geom->getMesh()->getTris(), _geom->getMesh()->getNormals(), _geom->getMesh()->getTriCount(),
                agentMaxSlope, texScale);
            _geom->drawOffMeshConnections(_dd.get());
        }

        glDepthMask(GL_FALSE);

        // Draw bounds
        const float* bmin = _geom->getNavMeshBoundsMin();
        const float* bmax = _geom->getNavMeshBoundsMax();
        duDebugDrawBoxWire(_dd.get(), bmin[0], bmin[1], bmin[2], bmax[0], bmax[1], bmax[2], duRGBA(255, 255, 255, 128), 1.0f);

        // Tiling grid.
        //int gw = 0, gh = 0;
        //rcCalcGridSize(bmin, bmax, m_cellSize, &gw, &gh);
        //const int tw = (gw + (int)m_tileSize - 1) / (int)m_tileSize;
        //const int th = (gh + (int)m_tileSize - 1) / (int)m_tileSize;
        //const float s = m_tileSize * m_cellSize;
        //duDebugDrawGridXZ(_dd.get(), bmin[0], bmin[1], bmin[2], tw, th, s, duRGBA(0, 0, 0, 64), 1.0f);

        //// Draw active tile
        //duDebugDrawBoxWire(_dd.get(), m_lastBuiltTileBmin[0], m_lastBuiltTileBmin[1], m_lastBuiltTileBmin[2],
        //    m_lastBuiltTileBmax[0], m_lastBuiltTileBmax[1], m_lastBuiltTileBmax[2], m_tileCol, 1.0f);

        if (_drawMode == DRAWMODE_NAVMESH ||
            _drawMode == DRAWMODE_NAVMESH_TRANS ||
            _drawMode == DRAWMODE_NAVMESH_BVTREE ||
            _drawMode == DRAWMODE_NAVMESH_NODES ||
            _drawMode == DRAWMODE_NAVMESH_PORTALS ||
            _drawMode == DRAWMODE_NAVMESH_INVIS)
        {
            const auto navMeshFlags = static_cast<unsigned char>(DrawNavMeshFlags{});

            if (_drawMode != DRAWMODE_NAVMESH_INVIS)
                duDebugDrawNavMeshWithClosedList(_dd.get(), _data.GetMesh(), _data.GetQuery(), navMeshFlags);
            if (_drawMode == DRAWMODE_NAVMESH_BVTREE)
                duDebugDrawNavMeshBVTree(_dd.get(), _data.GetMesh());
            if (_drawMode == DRAWMODE_NAVMESH_PORTALS)
                duDebugDrawNavMeshPortals(_dd.get(), _data.GetMesh());
            if (_drawMode == DRAWMODE_NAVMESH_NODES)
                duDebugDrawNavMeshNodes(_dd.get(), _data.GetQuery());
            duDebugDrawNavMeshPolysWithFlags(_dd.get(), _data.GetMesh(), SAMPLE_POLYFLAGS_DISABLED, duRGBA(0, 0, 0, 128));
        }


        glDepthMask(GL_TRUE);

        _geom->drawConvexVolumes(_dd.get());

        _testTool->handleRender();

        glDepthMask(GL_TRUE);
    }

    void Visualizer::handleRenderOverlay(double* proj, double* model, int* view)
    {
        _testTool->handleRenderOverlay(proj, model, view);
    }

    void Visualizer::handleUpdate(const float dt)
    {
        _testTool->handleUpdate(dt);
    }

    void Visualizer::RenderThreadMain(Promise<std::pair<bool, std::string>>& initPromise)
    {
        // Init SDL
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        {
            initPromise.Set(std::make_pair(false, fmt::format("Could not initialise SDL. SDL_Init Error: {}", SDL_GetError())));

            return;
        }

        // Use OpenGL render driver.
        SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

        // Enable depth buffer.
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        // Set color channel depth.
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

        // 4x MSAA.
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

        SDL_DisplayMode displayMode;
        SDL_GetCurrentDisplayMode(0, &displayMode);

        bool presentationMode = false;
        Uint32 flags = SDL_WINDOW_OPENGL;
        int width;
        int height;
        if (presentationMode)
        {
            // Create a fullscreen window at the native resolution.
            width = displayMode.w;
            height = displayMode.h;
            flags |= SDL_WINDOW_FULLSCREEN;
        }
        else
        {
            //float aspect = 16.0f / 9.0f;
            width = 1280;//rcMin(displayMode.w, (int)(displayMode.h * aspect)) - 80;
            height = 720;//displayMode.h - 80;
        }

        SDL_Window* window;
        SDL_Renderer* renderer;
        int errorCode = SDL_CreateWindowAndRenderer(width, height, flags, &window, &renderer);

        if (errorCode != 0 || !window || !renderer)
        {
            initPromise.Set(std::make_pair(false, fmt::format("Could not initialise SDL. CreateWindowAndRenderer Error: {}", SDL_GetError())));

            return;
        }

        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

        if (!imguiRenderGLInit("C:/Windows/Fonts/arial.ttf"))
        {
            initPromise.Set(std::make_pair(false, fmt::format("Could not init GUI renderer. imguiRenderGLInit")));

            return;
        }

        initPromise.Set(std::make_pair(true, ""));

        float timeAcc = 0.0f;
        Uint32 prevFrameTime = SDL_GetTicks();
        int mousePos[2] = { 0, 0 };
        int origMousePos[2] = { 0, 0 }; // Used to compute mouse movement totals across frames.

        float cameraEulers[] = { 45, -45 };
        float cameraPos[] = { 0, 0, 0 };
        float camr = 1000;
        float origCameraEulers[] = { 0, 0 }; // Used to compute rotational changes across frames.
        {
            const float* bmin = _geom->getNavMeshBoundsMin();
            const float* bmax = _geom->getNavMeshBoundsMax();

            // Reset camera and fog to match the mesh bounds.
            camr = sqrtf(rcSqr(bmax[0] - bmin[0]) +
                rcSqr(bmax[1] - bmin[1]) +
                rcSqr(bmax[2] - bmin[2])) / 2;
            cameraPos[0] = (bmax[0] + bmin[0]) / 2 + camr;
            cameraPos[1] = (bmax[1] + bmin[1]) / 2 + camr;
            cameraPos[2] = (bmax[2] + bmin[2]) / 2 + camr;
            camr *= 3;

            glFogf(GL_FOG_START, camr * 0.1f);
            glFogf(GL_FOG_END, camr * 1.25f);
        }

        float moveFront = 0.0f, moveBack = 0.0f, moveLeft = 0.0f, moveRight = 0.0f, moveUp = 0.0f, moveDown = 0.0f;

        float scrollZoom = 0;
        bool rotate = false;
        bool movedDuringRotate = false;
        float rayStart[3];
        float rayEnd[3];
        bool mouseOverMenu = false;

        bool showMenu = !presentationMode;
        bool showLog = false;
        bool showTools = true;
        //bool showLevels = false;
        //bool showSample = false;
        bool showTestCases = false;

        // Window scroll positions.
        int propScroll = 0;
        int logScroll = 0;
        int toolsScroll = 0;

        std::string sampleName = "Choose Sample...";

        std::vector<std::string> files;
        const std::string meshesFolder = "Meshes";
        std::string meshName = "Choose Mesh...";

        float markerPosition[3] = { 0, 0, 0 };
        bool markerPositionSet = false;

        const std::string testCasesFolder = "TestCases";
        TestCase* test = nullptr;

        // Fog.
        float fogColor[4] = { 0.32f, 0.31f, 0.30f, 1.0f };
        glEnable(GL_FOG);
        glFogi(GL_FOG_MODE, GL_LINEAR);
        glFogf(GL_FOG_START, camr * 0.1f);
        glFogf(GL_FOG_END, camr * 1.25f);
        glFogfv(GL_FOG_COLOR, fogColor);

        glEnable(GL_CULL_FACE);
        glDepthFunc(GL_LEQUAL);

        while (!_shutdown.load())
        {
            // Handle input events.
            int mouseScroll = 0;
            bool processHitTest = false;
            bool processHitTestShift = false;
            SDL_Event event;

            while (SDL_PollEvent(&event))
            {
                switch (event.type)
                {
                case SDL_KEYDOWN:
                    // Handle any key presses here.
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        _shutdown.store(true);
                    }
                    /*else if (event.key.keysym.sym == SDLK_t)
                    {
                        showLevels = false;
                        showSample = false;
                        showTestCases = true;
                        scanDirectory(testCasesFolder, ".txt", files);
                    }*/
                    else if (event.key.keysym.sym == SDLK_TAB)
                    {
                        showMenu = !showMenu;
                    }
                    else if (event.key.keysym.sym == SDLK_SPACE)
                    {
                        this->handleToggle();
                    }
                    else if (event.key.keysym.sym == SDLK_1)
                    {
                        this->handleStep();
                    }
                    /*else if (event.key.keysym.sym == SDLK_9)
                    {
                        if (sample && geom)
                        {
                            string savePath = meshesFolder + "/";
                            BuildSettings settings;
                            memset(&settings, 0, sizeof(settings));

                            rcVcopy(settings.navMeshBMin, geom->getNavMeshBoundsMin());
                            rcVcopy(settings.navMeshBMax, geom->getNavMeshBoundsMax());

                            sample->collectSettings(settings);

                            geom->saveGeomSet(&settings);
                        }
                    }*/
                    break;

                case SDL_MOUSEWHEEL:
                    if (event.wheel.y < 0)
                    {
                        // wheel down
                        if (mouseOverMenu)
                        {
                            mouseScroll++;
                        }
                        else
                        {
                            scrollZoom += 1.0f;
                        }
                    }
                    else
                    {
                        if (mouseOverMenu)
                        {
                            mouseScroll--;
                        }
                        else
                        {
                            scrollZoom -= 1.0f;
                        }
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_RIGHT)
                    {
                        if (!mouseOverMenu)
                        {
                            // Rotate view
                            rotate = true;
                            movedDuringRotate = false;
                            origMousePos[0] = mousePos[0];
                            origMousePos[1] = mousePos[1];
                            origCameraEulers[0] = cameraEulers[0];
                            origCameraEulers[1] = cameraEulers[1];
                        }
                    }
                    break;

                case SDL_MOUSEBUTTONUP:
                    // Handle mouse clicks here.
                    if (event.button.button == SDL_BUTTON_RIGHT)
                    {
                        rotate = false;
                        if (!mouseOverMenu)
                        {
                            if (!movedDuringRotate)
                            {
                                processHitTest = true;
                                processHitTestShift = true;
                            }
                        }
                    }
                    else if (event.button.button == SDL_BUTTON_LEFT)
                    {
                        if (!mouseOverMenu)
                        {
                            processHitTest = true;
                            processHitTestShift = (SDL_GetModState() & KMOD_SHIFT) ? true : false;
                        }
                    }

                    break;

                case SDL_MOUSEMOTION:
                    mousePos[0] = event.motion.x;
                    mousePos[1] = height - 1 - event.motion.y;

                    if (rotate)
                    {
                        int dx = mousePos[0] - origMousePos[0];
                        int dy = mousePos[1] - origMousePos[1];
                        cameraEulers[0] = origCameraEulers[0] - dy * 0.25f;
                        cameraEulers[1] = origCameraEulers[1] + dx * 0.25f;
                        if (dx * dx + dy * dy > 3 * 3)
                        {
                            movedDuringRotate = true;
                        }
                    }
                    break;

                case SDL_QUIT:
                    _shutdown.store(true);
                    break;

                default:
                    break;
                }
            }

            const auto tid = std::this_thread::get_id();
            while (!_strand.try_lock(tid))
            {
                std::this_thread::yield();
            }

            boost::scope::scope_exit exit([this, tid]()
                {
                    _strand.release(tid);
                });

            if (_shutdown.load())
            {
                break;
            }

            unsigned char mouseButtonMask = 0;
            if (SDL_GetMouseState(0, 0) & SDL_BUTTON_LMASK)
                mouseButtonMask |= IMGUI_MBUT_LEFT;
            if (SDL_GetMouseState(0, 0) & SDL_BUTTON_RMASK)
                mouseButtonMask |= IMGUI_MBUT_RIGHT;

            Uint32 time = SDL_GetTicks();
            float dt = (time - prevFrameTime) / 1000.0f;
            prevFrameTime = time;

            // Hit test mesh.
            if (processHitTest && _geom)
            {
                float hitTime;
                bool hit = _geom->raycastMesh(rayStart, rayEnd, hitTime);

                if (hit)
                {
                    if (SDL_GetModState() & KMOD_CTRL)
                    {
                        // Marker
                        markerPositionSet = true;
                        markerPosition[0] = rayStart[0] + (rayEnd[0] - rayStart[0]) * hitTime;
                        markerPosition[1] = rayStart[1] + (rayEnd[1] - rayStart[1]) * hitTime;
                        markerPosition[2] = rayStart[2] + (rayEnd[2] - rayStart[2]) * hitTime;
                    }
                    else
                    {
                        float pos[3];
                        pos[0] = rayStart[0] + (rayEnd[0] - rayStart[0]) * hitTime;
                        pos[1] = rayStart[1] + (rayEnd[1] - rayStart[1]) * hitTime;
                        pos[2] = rayStart[2] + (rayEnd[2] - rayStart[2]) * hitTime;

                        this->handleClick(rayStart, pos, processHitTestShift);
                    }
                }
                else
                {
                    if (SDL_GetModState() & KMOD_CTRL)
                    {
                        // Marker
                        markerPositionSet = false;
                    }
                }
            }

            // Update sample simulation.
            const float SIM_RATE = 20;
            const float DELTA_TIME = 1.0f / SIM_RATE;
            timeAcc = rcClamp(timeAcc + dt, -1.0f, 1.0f);
            int simIter = 0;
            while (timeAcc > DELTA_TIME)
            {
                timeAcc -= DELTA_TIME;
                if (simIter < 5)
                {
                    this->handleUpdate(DELTA_TIME);
                }
                simIter++;
            }

            

            // Clamp the framerate so that we do not hog all the CPU.
            const float MIN_FRAME_TIME = 1.0f / 40.0f;
            if (dt < MIN_FRAME_TIME)
            {
                int ms = (int)((MIN_FRAME_TIME - dt) * 1000.0f);
                if (ms > 10) ms = 10;
                if (ms >= 0) SDL_Delay(ms);
            }

            // Set the viewport.
            glViewport(0, 0, width, height);
            GLint viewport[4];
            glGetIntegerv(GL_VIEWPORT, viewport);

            // Clear the screen
            glClearColor(0.3f, 0.3f, 0.32f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDisable(GL_TEXTURE_2D);
            glEnable(GL_DEPTH_TEST);

            // Compute the projection matrix.
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(50.0f, (float)width / (float)height, 1.0f, camr);
            GLdouble projectionMatrix[16];
            glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);

            // Compute the modelview matrix.
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glRotatef(cameraEulers[0], 1, 0, 0);
            glRotatef(cameraEulers[1], 0, 1, 0);
            glTranslatef(-cameraPos[0], -cameraPos[1], -cameraPos[2]);
            GLdouble modelviewMatrix[16];
            glGetDoublev(GL_MODELVIEW_MATRIX, modelviewMatrix);

            // Get hit ray position and direction.
            GLdouble x, y, z;
            gluUnProject(mousePos[0], mousePos[1], 0.0f, modelviewMatrix, projectionMatrix, viewport, &x, &y, &z);
            rayStart[0] = (float)x;
            rayStart[1] = (float)y;
            rayStart[2] = (float)z;
            gluUnProject(mousePos[0], mousePos[1], 1.0f, modelviewMatrix, projectionMatrix, viewport, &x, &y, &z);
            rayEnd[0] = (float)x;
            rayEnd[1] = (float)y;
            rayEnd[2] = (float)z;

            // Handle keyboard movement.
            const Uint8* keystate = SDL_GetKeyboardState(NULL);
            moveFront = rcClamp(moveFront + dt * 4 * ((keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_UP]) ? 1 : -1), 0.0f, 1.0f);
            moveLeft = rcClamp(moveLeft + dt * 4 * ((keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_LEFT]) ? 1 : -1), 0.0f, 1.0f);
            moveBack = rcClamp(moveBack + dt * 4 * ((keystate[SDL_SCANCODE_S] || keystate[SDL_SCANCODE_DOWN]) ? 1 : -1), 0.0f, 1.0f);
            moveRight = rcClamp(moveRight + dt * 4 * ((keystate[SDL_SCANCODE_D] || keystate[SDL_SCANCODE_RIGHT]) ? 1 : -1), 0.0f, 1.0f);
            moveUp = rcClamp(moveUp + dt * 4 * ((keystate[SDL_SCANCODE_Q] || keystate[SDL_SCANCODE_PAGEUP]) ? 1 : -1), 0.0f, 1.0f);
            moveDown = rcClamp(moveDown + dt * 4 * ((keystate[SDL_SCANCODE_E] || keystate[SDL_SCANCODE_PAGEDOWN]) ? 1 : -1), 0.0f, 1.0f);

            float keybSpeed = 22.0f;
            if (SDL_GetModState() & KMOD_SHIFT)
            {
                keybSpeed *= 4.0f;
            }

            float movex = (moveRight - moveLeft) * keybSpeed * dt;
            float movey = (moveBack - moveFront) * keybSpeed * dt + scrollZoom * 2.0f;
            scrollZoom = 0;

            cameraPos[0] += movex * (float)modelviewMatrix[0];
            cameraPos[1] += movex * (float)modelviewMatrix[4];
            cameraPos[2] += movex * (float)modelviewMatrix[8];

            cameraPos[0] += movey * (float)modelviewMatrix[2];
            cameraPos[1] += movey * (float)modelviewMatrix[6];
            cameraPos[2] += movey * (float)modelviewMatrix[10];

            cameraPos[1] += (moveUp - moveDown) * keybSpeed * dt;

            glEnable(GL_FOG);

            this->handleRender();

            if (test)
                test->handleRender();

            Tick();

            glDisable(GL_FOG);

            // Render GUI
            glDisable(GL_DEPTH_TEST);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluOrtho2D(0, width, 0, height);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            mouseOverMenu = false;

            imguiBeginFrame(mousePos[0], mousePos[1], mouseButtonMask, mouseScroll);

            this->handleRenderOverlay((double*)projectionMatrix, (double*)modelviewMatrix, (int*)viewport);

            if (test)
            {
                if (test->handleRenderOverlay((double*)projectionMatrix, (double*)modelviewMatrix, (int*)viewport))
                    mouseOverMenu = true;
            }

            // Help text.
            if (showMenu)
            {
                const char msg[] = "W/S/A/D: Move  RMB: Rotate";
                imguiDrawText(280, height - 20, IMGUI_ALIGN_LEFT, msg, imguiRGBA(255, 255, 255, 128));
            }

            if (showMenu)
            {
                if (imguiBeginScrollArea("Properties", width - 250 - 10, 10, 250, height - 20, &propScroll))
                    mouseOverMenu = true;

                if (imguiCheck("Show Log", showLog))
                    showLog = !showLog;
                if (imguiCheck("Show Tools", showTools))
                    showTools = !showTools;

                imguiSeparator();
                /*imguiLabel("Sample");
                if (imguiButton(sampleName.c_str()))
                {
                    if (showSample)
                    {
                        showSample = false;
                    }
                    else
                    {
                        showSample = true;
                        showLevels = false;
                        showTestCases = false;
                    }
                }*/

                /*imguiSeparator();
                imguiLabel("Input Mesh");
                if (imguiButton(meshName.c_str()))
                {
                    if (showLevels)
                    {
                        showLevels = false;
                    }
                    else
                    {
                        showSample = false;
                        showTestCases = false;
                        showLevels = true;
                        scanDirectory(meshesFolder, ".obj", files);
                        scanDirectoryAppend(meshesFolder, ".gset", files);
                    }
                }*/

                if (_geom)
                {
                    char text[64];
                    snprintf(text, 64, "Verts: %.1fk  Tris: %.1fk",
                        _geom->getMesh()->getVertCount() / 1000.0f,
                        _geom->getMesh()->getTriCount() / 1000.0f);
                    imguiValue(text);
                }
                imguiSeparator();

                //if (geom && sample)
                //{
                //    imguiSeparatorLine();

                //    sample->handleSettings();

                //    if (imguiButton("Build"))
                //    {
                //        ctx.resetLog();
                //        if (!sample->handleBuild())
                //        {
                //            showLog = true;
                //            logScroll = 0;
                //        }
                //        ctx.dumpLog("Build log %s:", meshName.c_str());

                //        // Clear test.
                //        delete test;
                //        test = 0;
                //    }

                //    imguiSeparator();
                //}

                imguiSeparatorLine();
                this->handleDebugMode();

                imguiEndScrollArea();
            }

            // Sample selection dialog.
            //if (showSample)
            //{
            //    static int levelScroll = 0;
            //    if (imguiBeginScrollArea("Choose Sample", width - 10 - 250 - 10 - 200, height - 10 - 250, 200, 250, &levelScroll))
            //        mouseOverMenu = true;

            //    Sample* newSample = 0;
            //    for (int i = 0; i < g_nsamples; ++i)
            //    {
            //        if (imguiItem(g_samples[i].name.c_str()))
            //        {
            //            newSample = g_samples[i].create();
            //            if (newSample)
            //                sampleName = g_samples[i].name;
            //        }
            //    }
            //    if (newSample)
            //    {
            //        delete sample;
            //        sample = newSample;
            //        sample->setContext(&ctx);
            //        if (geom)
            //        {
            //            sample->handleMeshChanged(geom);
            //        }
            //        showSample = false;
            //    }

            //    if (geom || sample)
            //    {
            //        const float* bmin = 0;
            //        const float* bmax = 0;
            //        if (geom)
            //        {
            //            bmin = geom->getNavMeshBoundsMin();
            //            bmax = geom->getNavMeshBoundsMax();
            //        }
            //        // Reset camera and fog to match the mesh bounds.
            //        if (bmin && bmax)
            //        {
            //            camr = sqrtf(rcSqr(bmax[0] - bmin[0]) +
            //                rcSqr(bmax[1] - bmin[1]) +
            //                rcSqr(bmax[2] - bmin[2])) / 2;
            //            cameraPos[0] = (bmax[0] + bmin[0]) / 2 + camr;
            //            cameraPos[1] = (bmax[1] + bmin[1]) / 2 + camr;
            //            cameraPos[2] = (bmax[2] + bmin[2]) / 2 + camr;
            //            camr *= 3;
            //        }
            //        cameraEulers[0] = 45;
            //        cameraEulers[1] = -45;
            //        glFogf(GL_FOG_START, camr * 0.1f);
            //        glFogf(GL_FOG_END, camr * 1.25f);
            //    }

            //    imguiEndScrollArea();
            //}

            // Level selection dialog.
            //if (showLevels)
            //{
            //    static int levelScroll = 0;
            //    if (imguiBeginScrollArea("Choose Level", width - 10 - 250 - 10 - 200, height - 10 - 450, 200, 450, &levelScroll))
            //        mouseOverMenu = true;

            //    std::vector<std::string>::const_iterator fileIter = files.begin();
            //    std::vector<std::string>::const_iterator filesEnd = files.end();
            //    std::vector<std::string>::const_iterator levelToLoad = filesEnd;
            //    for (; fileIter != filesEnd; ++fileIter)
            //    {
            //        if (imguiItem(fileIter->c_str()))
            //        {
            //            levelToLoad = fileIter;
            //        }
            //    }

            //    if (levelToLoad != filesEnd)
            //    {
            //        meshName = *levelToLoad;
            //        showLevels = false;

            //        delete geom;
            //        geom = 0;

            //        std::string path = meshesFolder + "/" + meshName;

            //        geom = new InputGeom;
            //        if (!geom->load(&ctx, path))
            //        {
            //            delete geom;
            //            geom = 0;

            //            // Destroy the sample if it already had geometry loaded, as we've just deleted it!
            //            if (sample && sample->getInputGeom())
            //            {
            //                delete sample;
            //                sample = 0;
            //            }

            //            showLog = true;
            //            logScroll = 0;
            //            ctx.dumpLog("Geom load log %s:", meshName.c_str());
            //        }
            //        if (sample && geom)
            //        {
            //            sample->handleMeshChanged(geom);
            //        }

            //        if (geom || sample)
            //        {
            //            const float* bmin = 0;
            //            const float* bmax = 0;
            //            if (geom)
            //            {
            //                bmin = geom->getNavMeshBoundsMin();
            //                bmax = geom->getNavMeshBoundsMax();
            //            }
            //            // Reset camera and fog to match the mesh bounds.
            //            if (bmin && bmax)
            //            {
            //                camr = sqrtf(rcSqr(bmax[0] - bmin[0]) +
            //                    rcSqr(bmax[1] - bmin[1]) +
            //                    rcSqr(bmax[2] - bmin[2])) / 2;
            //                cameraPos[0] = (bmax[0] + bmin[0]) / 2 + camr;
            //                cameraPos[1] = (bmax[1] + bmin[1]) / 2 + camr;
            //                cameraPos[2] = (bmax[2] + bmin[2]) / 2 + camr;
            //                camr *= 3;
            //            }
            //            cameraEulers[0] = 45;
            //            cameraEulers[1] = -45;
            //            glFogf(GL_FOG_START, camr * 0.1f);
            //            glFogf(GL_FOG_END, camr * 1.25f);
            //        }
            //    }

            //    imguiEndScrollArea();

            //}

            // Test cases
            //if (showTestCases)
            //{
            //    static int testScroll = 0;
            //    if (imguiBeginScrollArea("Choose Test To Run", width - 10 - 250 - 10 - 200, height - 10 - 450, 200, 450, &testScroll))
            //        mouseOverMenu = true;

            //    std::vector<std::string>::const_iterator fileIter = files.begin();
            //    std::vector<std::string>::const_iterator filesEnd = files.end();
            //    std::vector<std::string>::const_iterator testToLoad = filesEnd;
            //    for (; fileIter != filesEnd; ++fileIter)
            //    {
            //        if (imguiItem(fileIter->c_str()))
            //        {
            //            testToLoad = fileIter;
            //        }
            //    }

            //    if (testToLoad != filesEnd)
            //    {
            //        std::string path = testCasesFolder + "/" + *testToLoad;
            //        test = new TestCase;
            //        if (test)
            //        {
            //            // Load the test.
            //            if (!test->load(path))
            //            {
            //                delete test;
            //                test = 0;
            //            }

            //            // Create sample
            //            Sample* newSample = 0;
            //            for (int i = 0; i < g_nsamples; ++i)
            //            {
            //                if (g_samples[i].name == test->getSampleName())
            //                {
            //                    newSample = g_samples[i].create();
            //                    if (newSample)
            //                        sampleName = g_samples[i].name;
            //                }
            //            }

            //            delete sample;
            //            sample = newSample;

            //            if (sample)
            //            {
            //                sample->setContext(&ctx);
            //                showSample = false;
            //            }

            //            // Load geom.
            //            meshName = test->getGeomFileName();


            //            path = meshesFolder + "/" + meshName;

            //            delete geom;
            //            geom = new InputGeom;
            //            if (!geom || !geom->load(&ctx, path))
            //            {
            //                delete geom;
            //                geom = 0;
            //                delete sample;
            //                sample = 0;
            //                showLog = true;
            //                logScroll = 0;
            //                ctx.dumpLog("Geom load log %s:", meshName.c_str());
            //            }
            //            if (sample && geom)
            //            {
            //                sample->handleMeshChanged(geom);
            //            }

            //            // This will ensure that tile & poly bits are updated in tiled sample.
            //            if (sample)
            //                sample->handleSettings();

            //            ctx.resetLog();
            //            if (sample && !sample->handleBuild())
            //            {
            //                ctx.dumpLog("Build log %s:", meshName.c_str());
            //            }

            //            if (geom || sample)
            //            {
            //                const float* bmin = 0;
            //                const float* bmax = 0;
            //                if (geom)
            //                {
            //                    bmin = geom->getNavMeshBoundsMin();
            //                    bmax = geom->getNavMeshBoundsMax();
            //                }
            //                // Reset camera and fog to match the mesh bounds.
            //                if (bmin && bmax)
            //                {
            //                    camr = sqrtf(rcSqr(bmax[0] - bmin[0]) +
            //                        rcSqr(bmax[1] - bmin[1]) +
            //                        rcSqr(bmax[2] - bmin[2])) / 2;
            //                    cameraPos[0] = (bmax[0] + bmin[0]) / 2 + camr;
            //                    cameraPos[1] = (bmax[1] + bmin[1]) / 2 + camr;
            //                    cameraPos[2] = (bmax[2] + bmin[2]) / 2 + camr;
            //                    camr *= 3;
            //                }
            //                cameraEulers[0] = 45;
            //                cameraEulers[1] = -45;
            //                glFogf(GL_FOG_START, camr * 0.2f);
            //                glFogf(GL_FOG_END, camr * 1.25f);
            //            }

            //            // Do the tests.
            //            if (sample)
            //                test->doTests(sample->getNavMesh(), sample->getNavMeshQuery());
            //        }
            //    }

            //    imguiEndScrollArea();
            //}


            // Log
            if (showLog && showMenu)
            {
                if (imguiBeginScrollArea("Log", 250 + 20, 10, width - 300 - 250, 200, &logScroll))
                    mouseOverMenu = true;
                //for (int i = 0; i < ctx.getLogCount(); ++i)
                    //imguiLabel(ctx.getLogText(i));
                imguiEndScrollArea();
            }

            // Left column tools menu
            if (!showTestCases && showTools && showMenu) // && geom && sample)
            {
                if (imguiBeginScrollArea("Tools", 10, 10, 250, height - 20, &toolsScroll))
                    mouseOverMenu = true;

                this->handleTools();

                imguiEndScrollArea();
            }

            // Marker
            if (markerPositionSet && gluProject((GLdouble)markerPosition[0], (GLdouble)markerPosition[1], (GLdouble)markerPosition[2],
                modelviewMatrix, projectionMatrix, viewport, &x, &y, &z))
            {
                // Draw marker circle
                glLineWidth(5.0f);
                glColor4ub(240, 220, 0, 196);
                glBegin(GL_LINE_LOOP);
                const float r = 25.0f;
                for (int i = 0; i < 20; ++i)
                {
                    const float a = (float)i / 20.0f * RC_PI * 2;
                    const float fx = (float)x + cosf(a) * r;
                    const float fy = (float)y + sinf(a) * r;
                    glVertex2f(fx, fy);
                }
                glEnd();
                glLineWidth(1.0f);
            }

            imguiEndFrame();
            imguiRenderGLDraw();

            glEnable(GL_DEPTH_TEST);
            SDL_GL_SwapWindow(window);
        }

        imguiRenderGLDestroy();

        SDL_Quit();
    }

    auto Visualizer::GetNavMesh() -> dtNavMesh&
    {
        return _data.GetMesh();
    }

    auto Visualizer::GetNavMeshQuery() -> dtNavMeshQuery&
    {
        return _data.GetQuery();
    }

    auto Visualizer::GetObjectFilePath() const -> const std::string&
    {
        return _data.GetObjectFilePath();
    }

    auto Visualizer::GetDebugDraw() -> duDebugDraw&
    {
        return *_dd;
    }

    auto Visualizer::GetAgentRadius() const -> float
    {
        return 35.f;
    }

    auto Visualizer::GetAgentHeight() const -> float
    {
        return 90.f;
    }

    auto Visualizer::GetAgentClimb() const -> float
    {
        return 0.9f;
    }
}
