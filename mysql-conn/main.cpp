#include <mysqlx/xdevapi.h>
#include <iostream>
#include <fmt/format.h>

typedef std::chrono::system_clock::time_point TimePoint;
int main(int argc, char* argv[]) {
    std::string portNo {"33061"};
    std::string db {"bkk_master"};
    for (int i = 1; i < argc; i++) {
        if (i == 1) portNo = argv[i];
        else if (i==2)
            db = argv[i];
    }
    try {
        auto url = fmt::format("mysqlx://root:ft1832h@127.0.0.1:{port}/{db}", fmt::arg("port", portNo), fmt::arg("db", db));
        std::cout << "Creating session on " << url << " ..." << std::endl;
        mysqlx::Client client(url);
        auto sess = client.getSession();
        std::cout << "Successfully connected to MySQL Server!" << std::endl;
        auto rs = sess.sql("select id, state_code, name from address_state;").execute();
        for (auto x : rs) {
            std::cout << x[0] << "> " << x[1] << "> " << x[2] << std::endl;
        }
        sess.close();

    } catch (const mysqlx::Error& err) {
        std::cerr << "MySQLX Error: " << err.what() << std::endl;
        return 1;
    } catch (const std::exception& ex) {
        std::cerr << "Standard Exception: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
