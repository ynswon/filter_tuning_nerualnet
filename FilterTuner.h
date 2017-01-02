#pragma once
class CFilterTuner
{
public:
	CFilterTuner(void);
	~CFilterTuner(void);
public:
	// 선호 아래(외부 호출 5 * 2048)
	// 메모리에 히든 레이어가 이미 올라와 있다고 가정하고
	// input만 넣어주는 행위
	// result에 출력
	void predict(double* main_data, double* sub_data, void* result);



	// 원준 아래
	// 학습 데이터 불러오기....(rda 파일 메모리에 불러오는 것과 같은 역할)
	void load_prediction_data(CString filter_name, CString tuning_phase);
	void learn_rt(int step);
	void learn_ft(int step); // ~~


private:
	// 원준이꺼
	
};

