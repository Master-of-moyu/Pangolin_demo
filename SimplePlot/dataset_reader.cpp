#include <iostream>
#include <thread>

#include <pangolin/pangolin.h>

using std::cout;
using std::endl;

struct RawDataReader {
    FILE *fp;

    RawDataReader(const std::string file_name) {
        std::string temp = (file_name).c_str();
        fp = fopen((file_name).c_str(), "rb");
        if (fp == NULL) {
            fprintf(stderr, "%s fopen error!\n", file_name.c_str());
        }
        std::cout << "test file opened success." << std::endl;
    }

    ~RawDataReader() {
        if (fp) {
            fclose(fp);
            fp = NULL;
        }
    }

    template <typename T>
    void Read(T *data, int size, const int N = 1) {
        fread(data, size, N, fp);
    }
};

int main() {
    // Create OpenGL window in single line
    pangolin::CreateWindowAndBind("Main", 1200, 500);

    // Data logger object
    pangolin::DataLog log;

    // Optionally add named labels
    std::vector<std::string> labels;
    // labels.push_back(std::string("sin(t)"));
    // labels.push_back(std::string("cos(t)"));
    // labels.push_back(std::string("sin(t)+cos(t)"));
    // labels.push_back(std::string("sin(t)-cos(t)"));
    labels.push_back(std::string("acc_time"));
    labels.push_back(std::string("gyro_time"));
    labels.push_back(std::string("grav_time"));
    log.SetLabels(labels);

    double base_time = 2776586.043414;
    // OpenGL 'view' of data. We might have many views of the same data.
    pangolin::Plotter plotter(&log, -0.3f, 50000, -10, 10000, 30, 0.5f);
    plotter.SetBounds(0.0, 1.0, 0.0, 1.0);
    plotter.Track("$i");

    // Add some sample annotations to the plot
    plotter.AddMarker(pangolin::Marker::Vertical, 0.3, pangolin::Marker::LessThan, pangolin::Colour::Blue().WithAlpha(0.2f));
    // plotter.AddMarker(pangolin::Marker::Horizontal, 100, pangolin::Marker::GreaterThan, pangolin::Colour::Red().WithAlpha(0.2f));
    // plotter.AddMarker(pangolin::Marker::Horizontal, 10, pangolin::Marker::Equal, pangolin::Colour::Green().WithAlpha(0.2f));

    pangolin::DisplayBase().AddDisplay(plotter);

    RawDataReader reader("/Users/shuxiangqian/DataSet/evaluate/temp/2020-03-20-16-35-48.sensors");
    double img_time, acc_time, gyro_time, gravity_time, rotation_time;
    double acc_data[3];
    double gyro_data[3];
    double gravity_data[3];
    double rotation_data[4];
    uint8_t type;
    int width, height;
    // Default hooks for exiting (Esc) and fullscreen (tab).
    while (!pangolin::ShouldQuit()) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // read data
        reader.Read<unsigned char>(&type, sizeof(uint8_t));
        if (type == 0) {
            reader.Read<double>(&img_time, sizeof(double));
            reader.Read<int>(&width, sizeof(int));
            reader.Read<int>(&height, sizeof(int));
            unsigned char *data = new unsigned char[width * height];
            reader.Read<unsigned char>(data, sizeof(unsigned char), width * height);
        } else if (type == 0x02) { // acc
            reader.Read<double>(&acc_time, sizeof(double));
            reader.Read<double>(acc_data, sizeof(double), 3);
        } else if (type == 0x01) { // gyro
            reader.Read<double>(&gyro_time, sizeof(double));
            reader.Read<double>(gyro_data, sizeof(double), 3);
        } else if (type == 0x12) { // gravity
            reader.Read<double>(&gravity_time, sizeof(double));
            reader.Read<double>(gravity_data, sizeof(double), 3);
        } else if (type == 0x11) { // rotation vector
            reader.Read<double>(&rotation_time, sizeof(double));
            reader.Read<double>(rotation_data, sizeof(double), 4);
        } else {
            cout << "wrong data type, quit" << endl;
            pangolin::ShouldQuit();
            break;
        }

        log.Log((acc_time - base_time) * 1000, (gyro_time - base_time) * 1000, (gravity_time - base_time) * 1000);
        // std::this_thread::sleep_for(std::chrono::milliseconds(10));

        // Render graph, Swap frames and Process Events
        pangolin::FinishFrame();
    }

    return 0;
}
