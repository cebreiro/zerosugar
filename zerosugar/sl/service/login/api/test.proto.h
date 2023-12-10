service
name: LoginService
full_name: zerosugar.sl.service.LoginService
    method
    name: SayHello
    full_name: zerosugar.sl.service.LoginService.SayHello
    debug_string: rpc SayHello(.zerosugar.sl.service.LoginRequest) returns (.zerosugar.sl.service.LoginReply);

        input_field
        name: testParam
        full_name: zerosugar.sl.service.LoginRequest.testParam
        type_name: int32
        cpp_type_name: int32

        output_field
        name: testResult
        full_name: zerosugar.sl.service.LoginReply.testResult
        type_name: int32
        cpp_type_name: int32


service
name: Greeter
full_name: zerosugar.sl.service.Greeter
    method
    name: SayHello
    full_name: zerosugar.sl.service.Greeter.SayHello
    debug_string: rpc SayHello(stream .zerosugar.sl.service.HelloRequest) returns (stream .zerosugar.sl.service.HelloReply);

        input_field
        name: name
        full_name: zerosugar.sl.service.HelloRequest.name
        type_name: string
        cpp_type_name: string

        output_field
        name: message
        full_name: zerosugar.sl.service.HelloReply.message
        type_name: string
        cpp_type_name: string


    method
    name: SayHelloAgain
    full_name: zerosugar.sl.service.Greeter.SayHelloAgain
    debug_string: rpc SayHelloAgain(.zerosugar.sl.service.HelloRequest) returns (.zerosugar.sl.service.HelloReply);

        input_field
        name: name
        full_name: zerosugar.sl.service.HelloRequest.name
        type_name: string
        cpp_type_name: string

        output_field
        name: message
        full_name: zerosugar.sl.service.HelloReply.message
        type_name: string
        cpp_type_name: string


