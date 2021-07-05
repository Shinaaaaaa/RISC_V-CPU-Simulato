//
// Created by lenovo on 2021/7/2.
//
#ifndef CODE_PREDICT_H
#define CODE_PREDICT_H

class predict{
    int status[500001]{};

public:
    predict(){
        for (int i = 0 ; i <= 500000 ; ++i) status[i] = 0;
    }

    int get_predict(unsigned int Pc){
        return status[Pc];
    }

    void predict_jump(unsigned int Pc){
        if (status[Pc] != 3) status[Pc]++;
    }

    void predict_stay(unsigned int Pc){
        if (status[Pc] != 0) status[Pc]--;
    }
};

#endif //CODE_PREDICT_H
