#include <iostream>
#include <thread>

#include <pangolin/pangolin.h>

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
    pangolin::CreateWindowAndBind("Main", 640, 480);

    // Data logger object
    pangolin::DataLog log;

    // Optionally add named labels
    std::vector<std::string> labels;
    labels.push_back(std::string("sin(t)"));
    labels.push_back(std::string("cos(t)"));
    labels.push_back(std::string("sin(t)+cos(t)"));
    labels.push_back(std::string("sin(t)-cos(t)"));
    log.SetLabels(labels);

    const float tinc = 0.005f;

    // OpenGL 'view' of data. We might have many views of the same data.
    pangolin::Plotter plotter(&log, -0.3f, 4.0f * (float)M_PI / tinc, -2.0f, 2.0f, (float)M_PI / (4.0f * tinc), 0.5f);
    plotter.SetBounds(0.0, 1.0, 0.0, 1.0);
    plotter.Track("$i");

    // Add some sample annotations to the plot
    plotter.AddMarker(pangolin::Marker::Vertical, 0.3, pangolin::Marker::LessThan, pangolin::Colour::Blue().WithAlpha(0.2f));
    plotter.AddMarker(pangolin::Marker::Horizontal, 100, pangolin::Marker::GreaterThan, pangolin::Colour::Red().WithAlpha(0.2f));
    plotter.AddMarker(pangolin::Marker::Horizontal, 10, pangolin::Marker::Equal, pangolin::Colour::Green().WithAlpha(0.2f));

    pangolin::DisplayBase().AddDisplay(plotter);

    float t = 0;

    // Default hooks for exiting (Esc) and fullscreen (tab).
    while (!pangolin::ShouldQuit()) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // log.Log(sin(t), cos(t));
        // log.Log(sin(t), cos(t), sin(t) + cos(t));
        log.Log(sin(t), cos(t), sin(t) + cos(t));
        t += tinc;
        std::this_thread::sleep_for(std::chrono::milliseconds(30));

        // Render graph, Swap frames and Process Events
        pangolin::FinishFrame();
    }

    return 0;
}
