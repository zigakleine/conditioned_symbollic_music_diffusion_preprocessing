#include <iostream>

extern "C" {
    struct ArrayInfo {
        int*** array;
        int dim1;
        int dim2;
        int dim3;
    };

    ArrayInfo generateArray() {
        int*** arr = new int**[3];
        for (int i = 0; i < 3; ++i) {
            arr[i] = new int*[3];
            for (int j = 0; j < 3; ++j) {
                arr[i][j] = new int[3];
                for (int k = 0; k < 3; ++k) {
                    arr[i][j][k] = 0;
                }
            }
        }

        ArrayInfo info;
        info.array = arr;
        info.dim1 = 3;
        info.dim2 = 3;
        info.dim3 = 3;
        return info;
    }
}
