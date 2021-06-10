<?
//==========================================================//
//	CX1000���������饹										//
//----------------------------------------------------------//
//	��������												//
//	��CX1000��												//
//	��ð�Ȼ��ѤȤ��ƺ�Ĵ���С������						//
//----------------------------------------------------------//
//	�ѹ�����												//
//		2019/02/13	N.ARAI									//
//		����������											//
//==========================================================//
class LoggerCX1000V2_TANBA2 extends LoggerCX1000V2
{
	//�ѿ����
	var $varsion	= "1.0";
	var $update		= "2019.02.13";
	var $memo		= "";
	var $id			= "CX1000V2_TANBA2";
	var $SettingTemplate = "lgr_cx1000v2.tmpl";
	var $cx1000_input_lgr_snow = false;	// ������������ܤ������������ؤ�̵ͭ��CX1000�����

	//Ģɼ��Ǥ��Υ����������ϤǤ�����ܤ����
	var $OutField = array(
		"000" => array(
			"avg_ws",
			"avg_wd",
			"max_ws",
			"max_wd",
			"temp",
			"humid1",
			"humid2",
			"rain_10min",
			"rain_hour",
			"rain_day",
			"move_rain_10min",
			"move_rain_hour",
			"move_rain_day",
			"rain_1min",
			"weather",
			"rain_rui"
		),
		"100" => array(
			"avg_ws",
			"avg_wd",
			"max_ws",
			"max_wd",
			"temp",
			"humid1",
			"humid2",
			"rain_10min",
			"rain_hour",
			"rain_day",
			"rain_10min_max",
			"rain_10min_max_time",
			"move_rain_hour_max",
			"move_rain_hour_max_time"
		),
		"200" => array(
			"avg_ws",
			"avg_wd",
			"max_ws",
			"max_wd",
			"temp",
			"humid1",
			"humid2",
			"max_rain_10min",
			"rain_hour",
			"rain_day",
			"weather"
		),
		"300" => array(
			"avg_ws",
			"avg_wd",
			"max_ws",
			"max_wd",
			"avg_temp",
			"max_temp",
			"min_temp",
			"avg_humid1",
			"max_humid1",
			"min_humid1",
			"humid2",
			"max_rain_10min",
			"max_rain_hour",
			"rain_day",
			"weather1",
			"weather2",
			"weather3"
		),
		"400" => array(
			"avg_ws",
			"avg_wd",
			"max_ws",
			"max_wd",
			"avg_temp",
			"max_temp",
			"min_temp",
			"avg_humid1",
			"max_humid1",
			"min_humid1",
			"humid2",
			"avg_humid2",
			"max_humid2",
			"min_humid2",
			"max_rain_10min",
			"max_rain_hour",
			"max_rain_day",
			"rain_month"
		)
	);

	//Ģɼ��Ǥ��Υ��������Խ��Ǥ�����ܤ����
	var $EditField = array(
		"100" => array(
			"avg_ws",
			"avg_wd",
			"max_ws",
			"max_wd",
			"temp",
			"humid1",
			"humid2",
			"rain_10min",
			"rain_hour",
			"rain_day",
			"rain_10min_max",
			"rain_10min_max_time",
			"move_rain_hour_max",
			"move_rain_hour_max_time"
		),
		"200" => array(
			"avg_ws",
			"avg_wd",
			"max_ws",
			"max_wd",
			"temp",
			"humid1",
			"humid2",
			"max_rain_10min",
			"rain_hour",
			"rain_day",
			"weather"
		)
	);
}
?>