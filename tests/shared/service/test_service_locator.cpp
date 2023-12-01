#include "zerosugar/shared/service/service_locator.h"

using zerosugar::IService;
using zerosugar::ServiceLocator;
using zerosugar::ServiceLocatorRef;

class FooService : public IService {};
class BarService : public IService {};
class BazService : public IService {};

TEST(ServiceLocatorTest, Add)
{
    // arrange
    auto foo = std::make_shared<FooService>();
    auto bar = std::make_shared<BarService>();
    ServiceLocator serviceLocator;

    // act
    bool addFoo = serviceLocator.Add<FooService>(foo);
    bool addBar = serviceLocator.Add<BarService>(bar);

    const FooService* findResultFoo = serviceLocator.Find<FooService>();
    const BarService* findResultBar = serviceLocator.Find<BarService>();
    const BazService* findResultBaz = serviceLocator.Find<BazService>();

    // assert
    EXPECT_TRUE(addFoo);
    EXPECT_TRUE(addBar);

    EXPECT_EQ(findResultFoo, foo.get());
    EXPECT_EQ(findResultBar, bar.get());
    EXPECT_EQ(findResultBaz, nullptr);
}

TEST(ServiceLocatorTest, AddDuplicated)
{
    // arrange
    auto foo = std::make_shared<FooService>();
    ServiceLocator serviceLocator;

    // act
    bool addFoo1 = serviceLocator.Add<FooService>(foo);
    bool addFoo2 = serviceLocator.Add<FooService>(foo);

    // assert
    EXPECT_TRUE(addFoo1);
    EXPECT_FALSE(addFoo2);
}

TEST(ServiceLocatorTest, AddAndRemove)
{
    // arrange
    auto foo = std::make_shared<FooService>();
    auto bar = std::make_shared<BarService>();
    ServiceLocator serviceLocator;

    // act
    const bool removeResult1 = serviceLocator.Remove<FooService>();

    serviceLocator.Add<FooService>(foo);
    const bool removeResult2 = serviceLocator.Remove<FooService>();

    const FooService* findResultFoo = serviceLocator.Find<FooService>();

    // assert
    EXPECT_FALSE(removeResult1);
    EXPECT_TRUE(removeResult2);
    EXPECT_EQ(findResultFoo, nullptr);
}

TEST(ServiceLocatorRef, ConstructAndFind)
{
    // arrange
    auto foo = std::make_shared<FooService>();
    auto bar = std::make_shared<BarService>();

    ServiceLocator serviceLocator;
    (void)serviceLocator.Add<FooService>(foo);
    (void)serviceLocator.Add<BarService>(bar);

    // act
    using service_locator_type = ServiceLocatorRef<FooService, BarService, BazService>;
    service_locator_type serviceLocatorT(serviceLocator);

    const FooService* fooService = serviceLocatorT.Find<FooService>();
    const BarService* barService = serviceLocatorT.Find<BarService>();
    const BazService* bazService = serviceLocatorT.Find<BazService>();

    // assert
    EXPECT_EQ(fooService, foo.get());
    EXPECT_EQ(barService, bar.get());
    EXPECT_EQ(bazService, nullptr);
}

TEST(ServiceLocatorRef, ConstructFromOther)
{
    // arrange
    auto foo = std::make_shared<FooService>();
    auto bar = std::make_shared<BarService>();

    ServiceLocator serviceLocator;
    (void)serviceLocator.Add<FooService>(foo);
    (void)serviceLocator.Add<BarService>(bar);

    // act
    ServiceLocatorRef<FooService, BarService, BazService> superset(serviceLocator);
    ServiceLocatorRef<FooService> subset1(superset);
    ServiceLocatorRef<BarService> subset2(superset);
    ServiceLocatorRef<BazService> subset3(superset);
    ServiceLocatorRef<FooService, BarService> subset4(superset);
    ServiceLocatorRef<FooService, BazService> subset5(superset);
    ServiceLocatorRef<BarService, BazService> subset6(superset);
    ServiceLocatorRef<FooService, BarService, BazService> subset7(superset);

    // assert
    EXPECT_EQ(subset1.Find<FooService>(), foo.get());
    EXPECT_EQ(subset2.Find<BarService>(), bar.get());
    EXPECT_EQ(subset3.Find<BazService>(), nullptr);

    EXPECT_EQ(subset4.Find<FooService>(), foo.get());
    EXPECT_EQ(subset4.Find<BarService>(), bar.get());

    EXPECT_EQ(subset5.Find<FooService>(), foo.get());
    EXPECT_EQ(subset5.Find<BazService>(), nullptr);

    EXPECT_EQ(subset6.Find<BarService>(), bar.get());
    EXPECT_EQ(subset6.Find<BazService>(), nullptr);

    EXPECT_EQ(subset7.Find<FooService>(), foo.get());
    EXPECT_EQ(subset7.Find<BarService>(), bar.get());
    EXPECT_EQ(subset7.Find<BazService>(), nullptr);
}
