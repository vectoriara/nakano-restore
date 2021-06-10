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
class LoggerCX1000V2_TANBA extends LoggerCX1000V2
{
	//�ѿ����
	var $varsion	= "1.0";
	var $update		= "2019.02.13";
	var $memo		= "";
	var $id			= "CX1000V2_TANBA";
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
			"press1",
			"press2",
			"rain_10min",
			"rain_hour",
			"rain_day",
			"move_rain_10min",
			"move_rain_hour",
			"move_rain_day",
			"rain_1min",
			"weather",
			"rain_rui",
			"ins_day",
			"sun_day",
		),
		"100" => array(
			"avg_ws",
			"avg_wd",
			"max_ws",
			"max_wd",
			"temp",
			"humid1",
			"humid2",
			"press1",
			"press2",
			"rain_10min",
			"rain_hour",
			"rain_day",
			"rain_10min_max",
			"rain_10min_max_time",
			"move_rain_hour_max",
			"move_rain_hour_max_time",
			"ins_day",
			"sun_day"
		),
		"200" => array(
			"avg_ws",
			"avg_wd",
			"max_ws",
			"max_wd",
			"temp",
			"humid1",
			"humid2",
			"press1",
			"press2",
			"max_rain_10min",
			"rain_hour",
			"rain_day",
			"weather",
			"ins_day",
			"sun_day"
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
			"press1",
			"avg_press1",
			"max_press1",
			"min_press1",
			"press2",
			"avg_press2",
			"max_press2",
			"min_press2",
			"max_rain_10min",
			"max_rain_hour",
			"rain_day",
			"weather1",
			"weather2",
			"weather3",
			"ins_day",
			"sun_day"
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
			"press1",
			"avg_press1",
			"max_press1",
			"min_press1",
			"press2",
			"avg_press2",
			"max_press2",
			"min_press2",
			"max_rain_10min",
			"max_rain_hour",
			"max_rain_day",
			"rain_month",
			"ins_month",
			"sun_month"
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
			"press1",
			"press2",
			"rain_10min",
			"rain_hour",
			"rain_day",
			"rain_10min_max",
			"rain_10min_max_time",
			"move_rain_hour_max",
			"move_rain_hour_max_time",
			"ins_day",
			"sun_day"
		),
		"200" => array(
			"avg_ws",
			"avg_wd",
			"max_ws",
			"max_wd",
			"temp",
			"humid1",
			"humid2",
			"press1",
			"press2",
			"max_rain_10min",
			"rain_hour",
			"rain_day",
			"weather",
			"ins_day",
			"sun_day"
		)
	);
}
?>