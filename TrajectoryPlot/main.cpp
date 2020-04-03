#include <iostream>
#include <thread>

#include <pangolin/pangolin.h>
#include <Eigen/Core>

using namespace Eigen;
using std::cout;
using std::endl;

// path to trajectory file
std::string trajectory_file = "./TrajectoryPlot/trajectory.tum";

void DrawTrajectory(std::vector<Isometry3d, Eigen::aligned_allocator<Isometry3d>> &);
void DrawTrajectoryCamera(std::vector<Isometry3d, Eigen::aligned_allocator<Isometry3d>> &);
void DrawPoints();

int main() {
    std::vector<Isometry3d, Eigen::aligned_allocator<Isometry3d>> poses;
    std::ifstream fin(trajectory_file);
    if (!fin) {
        cout << "cannot find trajectory file at " << trajectory_file << endl;
        return 1;
    }

    while (!fin.eof()) { //fin.eof()判断文件是否为空
        double time, tx, ty, tz, qx, qy, qz, qw;
        fin >> time >> tx >> ty >> tz >> qx >> qy >> qz >> qw;
        Isometry3d Twr(Quaterniond(qw, qx, qy, qz));        //变换矩阵的旋转部分
        Twr.pretranslate(Vector3d(tx * 3, ty * 3, tz * 3)); //变换矩阵的平移部分
        poses.push_back(Twr);
    }
    cout << "read total " << poses.size() << " pose entries" << endl;

    // draw trajectory in pangolin
    // DrawTrajectory(poses);
    DrawTrajectoryCamera(poses);
    return 0;
}

void DrawTrajectory(std::vector<Isometry3d, Eigen::aligned_allocator<Isometry3d>> &poses) {
    // create pangolin window and plot the trajectory
    pangolin::CreateWindowAndBind("Trajectory Viewer", 1024, 768);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    pangolin::OpenGlRenderState s_cam(
        pangolin::ProjectionMatrix(1024, 768, 500, 500, 512, 389, 0.1, 1000),
        pangolin::ModelViewLookAt(-2, 0, 3, 0, 0, 0, 1.0, 0.0, 0.0));

    pangolin::View &d_cam = pangolin::CreateDisplay()
                                .SetBounds(0.0, 1.0, 0.0, 1.0, -1024.0f / 768.0f)
                                .SetHandler(new pangolin::Handler3D(s_cam));

    while (pangolin::ShouldQuit() == false) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        d_cam.Activate(s_cam);
        glColor3f(0.5, 0.5, 0.5);
        glLineWidth(0.5);
        pangolin::glDraw_z0(0.5f, 10);
        glLineWidth(1);
        // 画每个位姿的三个坐标轴
        // for (size_t i = 0; i < poses.size(); i++) {
        //     Vector3d Ow = poses[i].translation();
        //     Vector3d Xw = poses[i] * (0.1 * Vector3d(1, 0, 0));
        //     Vector3d Yw = poses[i] * (0.1 * Vector3d(0, 1, 0));
        //     Vector3d Zw = poses[i] * (0.1 * Vector3d(0, 0, 1));
        //     glBegin(GL_LINES);
        //     glColor3f(1.0, 0.0, 0.0);
        //     glVertex3d(Ow[0], Ow[1], Ow[2]);
        //     glVertex3d(Xw[0], Xw[1], Xw[2]);
        //     glColor3f(0.0, 1.0, 0.0);
        //     glVertex3d(Ow[0], Ow[1], Ow[2]);
        //     glVertex3d(Yw[0], Yw[1], Yw[2]);
        //     glColor3f(0.0, 0.0, 1.0);
        //     glVertex3d(Ow[0], Ow[1], Ow[2]);
        //     glVertex3d(Zw[0], Zw[1], Zw[2]);
        //     glEnd();
        // }
        // 画出连线
        for (size_t i = 0; i < poses.size() - 1; i++) {
            glColor3f(1.0, 1.0, 0.0);
            glBegin(GL_LINES);
            auto p1 = poses[i], p2 = poses[i + 1];
            glVertex3d(p1.translation()[0], p1.translation()[1], p1.translation()[2]);
            glVertex3d(p2.translation()[0], p2.translation()[1], p2.translation()[2]);
            glEnd();
        }

        //画坐标原点
        glLineWidth(2);
        glBegin(GL_LINES);
        glColor3f(1.0, 0.0, 0.0);
        glVertex3d(0, 0, 0);
        glVertex3d(0.6, 0, 0);
        glColor3f(0.0, 1.0, 0.0);
        glVertex3d(0, 0, 0);
        glVertex3d(0, 0.6, 0);
        glColor3f(0.0, 0.0, 1.0);
        glVertex3d(0, 0, 0);
        glVertex3d(0, 0, 0.6);
        glEnd();
        pangolin::FinishFrame();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

void DrawTrajectoryCamera(std::vector<Isometry3d, Eigen::aligned_allocator<Isometry3d>> &poses) {
    if (poses.empty()) {
        cout << "parameter is empty!" << endl;
        return;
    }
    // create pangolin window and plot the trajectory
    pangolin::CreateWindowAndBind("Trajectory Viewer", 1024, 768);
    glEnable(GL_DEPTH_TEST); //深度测试
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    pangolin::OpenGlRenderState s_cam( //摆放一个相机
        pangolin::ProjectionMatrix(1024, 768, 500, 500, 512, 389, 0.1, 1000),
        pangolin::ModelViewLookAt(-1, 0, 3, 0, 0, 0, 1.0, 0.0, 0.0));
    pangolin::View &d_cam = pangolin::CreateDisplay() //创建一个窗口
                                .SetBounds(0.0, 1.0, pangolin::Attach::Pix(175), 1.0, -1024.0f / 768.0f)
                                .SetHandler(new pangolin::Handler3D(s_cam));

    while (pangolin::ShouldQuit() == false) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //消除颜色缓冲
        d_cam.Activate(s_cam);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glColor3f(0.5, 0.5, 0.5);
        glLineWidth(0.5);
        pangolin::glDraw_z0(0.5f, 10);
        // draw poses
        size_t i = 0;
        for (auto &Twc : poses) //从poses中取位姿
        {
            i++;
            if (i % 30 != 0) continue;
            glPushMatrix();
            Eigen::Matrix4f m = poses[i].matrix().cast<float>();
            glMultMatrixf((GLfloat *)m.data());

            const float w = 0.1;
            const float h = w * 0.75;
            const float z = w * 0.6;
            glColor3f(1, 0, 0);
            glLineWidth(2);
            glBegin(GL_LINES);
            //画相机模型
            glVertex3f(0, 0, 0);
            glVertex3f(w, h, z);
            glVertex3f(0, 0, 0);
            glVertex3f(w, -h, z);
            glVertex3f(0, 0, 0);
            glVertex3f(-w, -h, z);
            glVertex3f(0, 0, 0);
            glVertex3f(-w, h, z);
            glVertex3f(w, h, z);
            glVertex3f(w, -h, z);
            glVertex3f(-w, h, z);
            glVertex3f(-w, -h, z);
            glVertex3f(-w, h, z);
            glVertex3f(w, h, z);
            glVertex3f(-w, -h, z);
            glVertex3f(w, -h, z);

            glEnd();
            glPopMatrix();
        }
        //画轨迹
        glLineWidth(1);
        for (size_t i = 0; i < poses.size() - 1; i++) {
            glColor3f(1.0, 1.0, 0.0);
            glBegin(GL_LINES);
            auto p1 = poses[i], p2 = poses[i + 1];
            glVertex3d(p1.translation()[0], p1.translation()[1], p1.translation()[2]);
            glVertex3d(p2.translation()[0], p2.translation()[1], p2.translation()[2]);
        }
        glEnd();
        //画坐标原点
        glLineWidth(2);
        glBegin(GL_LINES);
        glColor3f(1.0, 0.0, 0.0);
        glVertex3d(0, 0, 0);
        glVertex3d(0.6, 0, 0);
        glColor3f(0.0, 1.0, 0.0);
        glVertex3d(0, 0, 0);
        glVertex3d(0, 0.6, 0);
        glColor3f(0.0, 0.0, 1.0);
        glVertex3d(0, 0, 0);
        glVertex3d(0, 0, 0.6);
        glEnd();
        DrawPoints();
        pangolin::FinishFrame();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

void DrawPoints() {
    const int N = 20;
    std::vector<Eigen::Vector3d> points;
    for (size_t i = 0; i < N / 2; i++) {
        Eigen::Vector3d p((i + 0.5) * 1.0 / 6, 0.2, 0.5);
        points.push_back(p);
    }

    for (size_t i = 0; i < N / 2; i++) {
        Eigen::Vector3d p(0.5, (i + 0.2) * 1.0 / 6, 0.5);
        points.push_back(p);
    }

    glPointSize(3);
    glBegin(GL_POINTS);
    glColor3f(1.0, 1.0, 1.0);

    for (size_t i = 0; i < N; i++) {
        glVertex3f(points[i][0], points[i][1], points[i][2]);
    }
    glEnd();
}
