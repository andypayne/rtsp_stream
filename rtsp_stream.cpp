#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>


enum TriState {
  ts_0,
  ts_1,
  ts_2 
};

void putBorderedText(
  cv::Mat&           img,
  const std::string& text,
  cv::Point          origin,
  int                fontFace,
  double             fontScale,
  cv::Scalar         color,
  int                thickness = 1,
  int                lineType = 8,
  bool               bottomLeftOrigin = false,
  cv::Scalar         borderColor = cv::Scalar::all(50)
) {
  cv::putText(img, text, origin, fontFace, fontScale, borderColor, 2.5 * thickness, lineType, bottomLeftOrigin);
  cv::putText(img, text, origin, fontFace, fontScale, color, thickness, lineType, bottomLeftOrigin);
}

constexpr int ESC_KEY = 27;
constexpr int PAUSE_KEY = ' ';
constexpr int FULLSCREEN_KEY = 'f';


int main(int argc, char* argv[]) {
  using std::cout;
  using std::endl;
  if (argc != 2) {
    cout <<" Usage: " << argv[0] << " <rtsp url>" << endl;
    return -1;
  }
  const char *url{argv[1]};
  cv::VideoCapture capture(url);
  if (!capture.isOpened()) {
    cout << "Error: capture not opened." << endl;
    return -1;
  }
  cv::namedWindow("rtsp_stream", cv::WINDOW_NORMAL);
  cv::Mat frame;
  bool end_stream = false;
  TriState stream_paused = ts_0;
  bool full_screen = false;
  const std::string help_text{"[Esc = quit, Space = pause, f = toggle fullscreen]"};
  std::string pause_status{""};
  while (!end_stream) {
    if (stream_paused == ts_0) {
      if (!capture.read(frame)) {
        cout << "Error: unable to read a frame from capture." << endl;
        return -1;
      }
      cv::Point help_text_origin(0.05 * frame.cols, 30);
      putBorderedText(frame, help_text, help_text_origin, cv::FONT_HERSHEY_DUPLEX, 0.75, cv::Scalar::all(255), 2, 4);
      cv::imshow("rtsp_stream", frame);
    } else if (stream_paused == ts_1) {
      stream_paused = ts_2;
      cv::Point help_text_origin(0.05 * frame.cols, 30);
      cv::Point pause_status_origin(0.9 * frame.cols, 30);
      putBorderedText(frame, pause_status, pause_status_origin, cv::FONT_HERSHEY_DUPLEX, 0.75, cv::Scalar::all(255), 2, 4);
      cv::imshow("rtsp_stream", frame);
      if (!capture.read(frame)) {
        cout << "Error: unable to read a frame from capture." << endl;
        return -1;
      }
    }
    int k = cv::waitKey(30);
    if (k == ESC_KEY) {
      end_stream = true;
    } else if (k == FULLSCREEN_KEY) {
      full_screen = !full_screen;
      if (full_screen) {
        cv::setWindowProperty("rtsp_stream", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
      } else {
        cv::setWindowProperty("rtsp_stream", cv::WND_PROP_FULLSCREEN, cv::WINDOW_NORMAL);
      }
    } else if (k == PAUSE_KEY) {
      switch(stream_paused) {
        case ts_0 :
          stream_paused = ts_1;
          pause_status = "paused";
          break;
        case ts_1 :
          stream_paused = ts_0;
          pause_status = "";
          break;
        case ts_2 :
          stream_paused = ts_0;
          pause_status = "";
          break;
      }
    }
  }
  return 0;
}

