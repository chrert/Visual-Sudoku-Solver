#ifndef SETTINGS_HPP__
#define SETTINGS_HPP__

#define CAM_NUM 0

#define SUDOKU_CELL_WORKING_SIZE 40

#define DIGIT_SAMPLE_WIDTH 16
#define PCA_COMPONENTS 0

#define KNN_K 4

#define NO_DIGIT_FOUND 0

#define CANNY_LOW  40
#define CANNY_HIGH 80

#define MIN_CONTOUR_AREA 1000

#define NUM_ROWS_CELLS 9
#define BOX_WIDTH 0
#define BOX_HEIGHT 0

#ifndef TRAINING_DATA_DIR
  #define TRAINING_DATA_DIR "../../training_set/"
#endif

#define NUM_FRAMES_FIXED 15
#define NUM_FRAMES_LOST 10

#endif
