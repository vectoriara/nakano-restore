<?
//==========================================================//
//	CX1000���������饹										//
//----------------------------------------------------------//
//	��������												//
//	��CX1000��												//
//	����ë�Ա��̶��ѥ��饹									//
//----------------------------------------------------------//
//	�ѹ�����												//
//		2020/07/08	N.ARAI									//
//		����������											//
//==========================================================//
class LoggerCX1000V2_RAIN extends LoggerCX1000V2
{
	//�ѿ����
	var $varsion	= "1.0";
	var $update		= "2020.07.08";
	var $memo		= "";
	var $id			= "CX1000V2_RAIN";
	var $type		= "2";	//�����������ס�1 => ���ݥ�����	 2 => ���̥�����

	var $SettingTemplate = "lgr_cx1000v2.tmpl";

	var $cx1000_input_rain_mode = false;	// ������������ܤ˱����������ؤ�̵ͭ��CX1000�����
	var $cx1000_input_lgr_snow = false;	// ������������ܤ������������ؤ�̵ͭ��CX1000�����

	var $EditSys	= array("100","200");	//�Խ���ǽ�ʥǡ���Ģɼ
	var $ColSys		= array("100");			//��ư������ǽ�ʥǡ���Ģɼ
	var $DelSys		= array("100","200");	//�����ǽ�ʥǡ���Ģɼ

	var $Recal1minFlg	= true;				//1ʬ�ǡ�������Ѥ��Ƥη׻�����ǽ���Բ�ǽ
	var $RecalDisregard	= false;			//1ʬ�ǡ��������å���̵�뤷�ƽ����򤹤뤫�βġ���

	//Ģɼ��Ǥ��Υ����������ϤǤ�����ܤ����
	var $OutField = array(
		"000" => array(
			"rain_10min",
			"rain_hour",
			"rain_day",
			"move_rain_10min",
			"move_rain_hour",
			"move_rain_day",
			"rain_1min",
			"rain_rui"
		),
		"100" => array(
			"rain_10min",
			"rain_hour",
			"rain_day",
			"rain_10min_max",
			"rain_10min_max_time",
			"move_rain_hour_max",
			"move_rain_hour_max_time"
		),
		"200" => array(
			"max_rain_10min",
			"rain_hour",
			"rain_day"
		),
		"300" => array(
			"max_rain_10min",
			"max_rain_hour",
			"rain_day"
		),
		"400" => array(
			"max_rain_10min",
			"max_rain_hour",
			"max_rain_day",
			"rain_month"
		)
	);

	//Ģɼ��Ǥ��Υ��������Խ��Ǥ�����ܤ����
	var $EditField = array(
		"100" => array(
			"rain_10min",
			"rain_hour",
			"rain_day",
			"rain_10min_max",
			"rain_10min_max_time",
			"move_rain_hour_max",
			"move_rain_hour_max_time"
		),
		"200" => array(
			"max_rain_10min",
			"rain_hour",
			"rain_day"
		)
	);
}
?>