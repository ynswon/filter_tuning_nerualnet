#pragma once
class CFilterTuner
{
public:
	CFilterTuner(void);
	~CFilterTuner(void);
public:
	// ��ȣ �Ʒ�(�ܺ� ȣ�� 5 * 2048)
	// �޸𸮿� ���� ���̾ �̹� �ö�� �ִٰ� �����ϰ�
	// input�� �־��ִ� ����
	// result�� ���
	void predict(double* main_data, double* sub_data, void* result);



	// ���� �Ʒ�
	// �н� ������ �ҷ�����....(rda ���� �޸𸮿� �ҷ����� �Ͱ� ���� ����)
	void load_prediction_data(CString filter_name, CString tuning_phase);
	void learn_rt(int step);
	void learn_ft(int step); // ~~


private:
	// �����̲�
	
};

