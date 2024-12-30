//
// Created by ByteDance on 12/30/24.
//

#ifndef VIEW_POINT_CLOUD_H
#define VIEW_POINT_CLOUD_H

#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <iostream>
#include <vector>
#include <random>

namespace service::pcls::point_cloud {
    struct Point3D {
        double x, y, z;
        Point3D(double x, double y, double z) : x(x), y(y), z(z) {}
    };

    // 函数将 std::vector<std::vector<Point_3d>> 转换为 pcl::PointCloud<pcl::PointXYZ>
    pcl::PointCloud<pcl::PointXYZ>::Ptr convertToPCLPointCloud(const std::vector<Point3D>& points) {
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
        cloud->resize(points.size()); // 设置点云大小

        for (size_t i = 0; i < points.size(); ++i) {
            cloud->points[i].x = points[i].x;
            cloud->points[i].y = points[i].y;
            cloud->points[i].z = points[i].z;
        }

        cloud->width = static_cast<uint32_t>(points.size());
        cloud->height = 1; // 1 表示点云是单行的
        cloud->is_dense = false; // 可以设置为 true 如果没有 NaN 点

        return cloud;
    }



    /**
     * @brief 可视化点云数据
     * @param point_clouds 点云数据
     */
    void View_point_clouds(const std::vector<std::vector<Point3D>>& point_clouds) {
        // 创建可视化对象
        pcl::visualization::PCLVisualizer viewer("Point Cloud Viewer");
        // 定义颜色
        std::vector<std::tuple<float, float, float>> colors(point_clouds.size());
        auto generateRandomColor = [&]() -> std::tuple<float, float, float> {
            // 使用随机数生成器
            static std::random_device rd;  // 获取随机数种子
            static std::mt19937 gen(rd());  // 初始化随机数生成器
            std::uniform_real_distribution<float> dis(0.0f, 1.0f); // 均匀分布在 [0.0, 1.0] 之间

            float r = dis(gen);
            float g = dis(gen);
            float b = dis(gen);
            return std::make_tuple(r, g, b);
        };
        for (size_t i = 0; i < point_clouds.size(); ++i) {
            colors[i] = generateRandomColor();
        }

        // 遍历每个点云
        for (size_t i = 0; i < point_clouds.size(); ++i) {
            // 转换点云数据
            pcl::PointCloud<pcl::PointXYZ>::Ptr cloud = convertToPCLPointCloud(point_clouds[i]);

            // 添加点云到可视化器，并设置颜色
            viewer.addPointCloud<pcl::PointXYZ>(cloud, "cloud" + std::to_string(i));
            viewer.setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR,
                                                    std::get<0>(colors[i]),
                                                    std::get<1>(colors[i]),
                                                    std::get<2>(colors[i]),
                                                    "cloud" + std::to_string(i));
            // 设置点的大小
            viewer.setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 5, "cloud" + std::to_string(i)); // 这里的 5 是点的大小
        }

        // 设置视图
        viewer.setBackgroundColor(0.1, 0.1, 0.1); // 设置背景色为深灰色
        viewer.addCoordinateSystem(1.0); // 添加坐标轴
        viewer.initCameraParameters();

        // 进入可视化循环
        while (!viewer.wasStopped()) {
            viewer.spinOnce(100);
        }
    }
}

#endif //VIEW_POINT_CLOUD_H
