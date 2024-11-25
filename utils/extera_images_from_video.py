import os
import cv2
import matplotlib.pyplot as plt


def create_directory_structure(source_root, target_root):
    """创建与源文件夹结构相同的输出文件夹结构"""
    for root, dirs, files in os.walk(source_root):
        relative_path = os.path.relpath(root, source_root)
        target_path = os.path.join(target_root, relative_path)
        os.makedirs(target_path, exist_ok=True)


# interval抽帧间隔


def video_capture(video_path, output_root, source_root, interval=10):
    cap = cv2.VideoCapture(video_path)
    if not cap.isOpened():
        print(f"Error opening video file: {video_path}")
        return
    frame_count = 0
    saved_frame_count = 0
    relative_video_dir = os.path.dirname(os.path.relpath(video_path, start=source_root))
    output_dir = os.path.join(output_root, relative_video_dir)
    total_frames = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
    while True:
        ret, frame = cap.read()
        if not ret:
            break
        if frame_count % interval == 0:
            frame_filename = f"{os.path.splitext(os.path.basename(video_path))[0]}_{frame_count:04d}.jpg"
            frame_path = os.path.join(output_dir, frame_filename)
            cv2.imwrite(frame_path, frame)
            saved_frame_count += 1
        frame_count += 1
    cap.release()
    print(
        f"Processed and saved {saved_frame_count} out of {frame_count} frames from {video_path}"
    )


def count_files_in_directories(root_directory):
    """统计每个子目录下的文件数量，忽略根目录"""
    counts = {}
    for root, dirs, files in os.walk(root_directory):
        relative_path = os.path.relpath(root, root_directory)
        # 忽略根目录
        if relative_path == ".":
            continue
        counts[relative_path] = len(files)
    return counts


def plot_category_counts(category_counts):
    """绘制类别数量的柱状图"""
    categories = list(category_counts.keys())
    counts = list(category_counts.values())
    plt.figure(figsize=(10, 6))
    plt.bar(categories, counts, color="skyblue")
    plt.xlabel("Categories")
    plt.ylabel("Number of Images")
    plt.title("Image Count per Category")
    plt.xticks(rotation=45, ha="right")
    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    # 源视频文件夹路径
    video_root = r"C:\Users\Administrator\Desktop\new"
    # 输出图片文件夹根路径
    output_root = "face_add"
    if not os.path.exists(output_root):
        os.mkdir(output_root)
    # 创建与视频文件夹结构相同的输出文件夹结构
    create_directory_structure(video_root, output_root)

    # 获取所有视频文件
    video_files = [
        os.path.join(root, file)
        for root, _, files in os.walk(video_root)
        for file in files
        if file.lower().endswith((".mp4", ".avi", ".mov", ".mkv"))
    ]

    # 处理所有视频文件
    for video_file in video_files:
        video_capture(video_file, output_root, video_root)

    # 统计处理后各类别的数量
    category_counts = count_files_in_directories(output_root)
    for category, count in category_counts.items():
        print(f"Category '{category}' contains {count} images.")

    # 绘制柱状图
    plot_category_counts(category_counts)
