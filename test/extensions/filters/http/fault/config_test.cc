#include "extensions/filters/http/fault/config.h"

#include "test/mocks/server/mocks.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using testing::Invoke;
using testing::_;

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace Fault {

TEST(FaultFilterConfigTest, ValidateFail) {
  NiceMock<Server::Configuration::MockFactoryContext> context;
  envoy::config::filter::http::fault::v2::HTTPFault fault;
  fault.mutable_abort();
  EXPECT_THROW(FaultFilterFactory().createFilterFactoryFromProto(fault, "stats", context),
               ProtoValidationException);
}

TEST(FaultFilterConfigTest, FaultFilterCorrectJson) {
  std::string json_string = R"EOF(
  {
    "delay" : {
      "type" : "fixed",
      "fixed_delay_percent" : 100,
      "fixed_duration_ms" : 5000
    }
  }
  )EOF";

  Json::ObjectSharedPtr json_config = Json::Factory::loadFromString(json_string);
  NiceMock<Server::Configuration::MockFactoryContext> context;
  FaultFilterFactory factory;
  Server::Configuration::HttpFilterFactoryCb cb =
      factory.createFilterFactory(*json_config, "stats", context);
  Http::MockFilterChainFactoryCallbacks filter_callback;
  EXPECT_CALL(filter_callback, addStreamDecoderFilter(_));
  cb(filter_callback);
}

TEST(FaultFilterConfigTest, FaultFilterCorrectProto) {
  envoy::config::filter::http::fault::v2::HTTPFault config{};
  config.mutable_delay()->set_percent(100);
  config.mutable_delay()->mutable_fixed_delay()->set_seconds(5);

  NiceMock<Server::Configuration::MockFactoryContext> context;
  FaultFilterFactory factory;
  Server::Configuration::HttpFilterFactoryCb cb =
      factory.createFilterFactoryFromProto(config, "stats", context);
  Http::MockFilterChainFactoryCallbacks filter_callback;
  EXPECT_CALL(filter_callback, addStreamDecoderFilter(_));
  cb(filter_callback);
}

TEST(FaultFilterConfigTest, InvalidFaultFilterInProto) {
  envoy::config::filter::http::fault::v2::HTTPFault config{};
  NiceMock<Server::Configuration::MockFactoryContext> context;
  FaultFilterFactory factory;
  EXPECT_THROW(factory.createFilterFactoryFromProto(config, "stats", context), EnvoyException);
}

TEST(FaultFilterConfigTest, FaultFilterEmptyProto) {
  NiceMock<Server::Configuration::MockFactoryContext> context;
  FaultFilterFactory factory;
  EXPECT_THROW(
      factory.createFilterFactoryFromProto(*factory.createEmptyConfigProto(), "stats", context),
      EnvoyException);
}

} // namespace Fault
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
