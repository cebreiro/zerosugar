#include "service_packet_builder.h"

#include "zerosugar/xr/network/model/generated/service_to_service_generated.h"

namespace zerosugar::xr
{
    auto ServicePacketBuilder::MakePacket(const network::service::RequestRemoteProcedureCall& item) -> Buffer
    {
        Buffer buffer;
        buffer.Add(buffer::Fragment::Create(512));

        Buffer head;
        [[maybe_unused]] bool result = buffer.SliceFront(head, 4);
        assert(result);

        const int64_t bodySize = [&]()
            {
                BufferWriter writer(buffer);

                item.Serialize(writer);

                return writer.GetWriteSize();
            }();

        Buffer body;
        result = buffer.SliceFront(body, bodySize);
        assert(result);

        {
            BufferWriter headWriter(head);
            headWriter.Write<int32_t>(static_cast<int32_t>(body.GetSize()) + 4);

            assert(body.GetSize() == bodySize);
        }

        head.MergeBack(std::move(body));

        return head;
    }

    auto ServicePacketBuilder::MakePacket(const network::service::ResultRemoteProcedureCall& item) -> Buffer
    {
        Buffer buffer;
        buffer.Add(buffer::Fragment::Create(512));

        Buffer head;
        [[maybe_unused]] bool result = buffer.SliceFront(head, 4);
        assert(result);

        const int64_t bodySize = [&]()
            {
                BufferWriter writer(buffer);

                item.Serialize(writer);

                return writer.GetWriteSize();
            }();

        Buffer body;
        result = buffer.SliceFront(body, bodySize);
        assert(result);

        {
            BufferWriter headWriter(head);
            headWriter.Write<int32_t>(static_cast<int32_t>(body.GetSize()) + 4);

            assert(body.GetSize() == bodySize);
        }

        head.MergeBack(std::move(body));

        return head;
    }
}
