<?
class LoggerCX1000FTPTawaratsu extends LoggerCX1000FTP{
	//�ѿ����
	var $setting	= array();	//��������������Ǽ������
	var $load_flg	= false;	//��������������ɹ��ߥե饰
	var $varsion	= "1.0";
	var $update = "2012.12.19";
	var $memo = "�ˤ��ۻ����ѥ��饹������Ѥ���¼��";
	var $id	 = "CX1000FTPTawaratsu";
	var $map = array(
							"lgr_id" => "lgr_id",
							"lgr_pnt_id"	=> "lgr_pnt_id",
							"lgr_enable"	=> "lgr_enable",
							"lgr_time" => "lgr_time",
							"lgr_ftp_ip"	=> "lgr_col_1",
							"lgr_ftp_id"	=> "lgr_col_2",
							"lgr_ftp_pwd"	=> "lgr_col_3",
							"lgr_ftp_dir"	=> "lgr_col_4"
							);
	var $EditSys	= array();							//�Խ���ǽ�ʥǡ���Ģɼ
	var $ColSys = array("100","200","300","400");	//��ư������ǽ�ʥǡ���Ģɼ
	var $DelSys = array("100","200","300","400");	//�����ǽ�ʥǡ���Ģɼ

	var $flg_edit_rainrui = true;//�߷ױ����Խ�
	var $flg_edit_weather = false;//ŷ���Խ�
	var $flg_edit_warning = false;//����ģ�Խ�
	var $flg_edit_alert	 = true;//�����Խ�

	//Ģɼ��Ǥ��Υ����������ϤǤ�����ܤ����
	var $OutField = array(
		"000" => array(
						"rain_day",
						"move_rain_10min",
						"move_rain_hour",
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
						"press1",
						"rain_10min",
						"rain_hour",
						"rain_day",
						"press2"
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
						"max_rain_10min",
						"rain_hour",
						"rain_day",
						"press2"
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
						"max_rain_10min",
						"max_rain_hour",
						"rain_day",
						"press2"
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
						"max_rain_10min",
						"max_rain_hour",
						"max_rain_day",
						"rain_month",
						"press2",
		),
	);
	var $monthly_mode	= false;
	var $yearly_mode	= false;
}
?>