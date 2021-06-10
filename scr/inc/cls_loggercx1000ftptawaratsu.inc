<?
class LoggerCX1000FTPTawaratsu extends LoggerCX1000FTP{
	//変数宣言
	var $setting	= array();	//ロガー設定情報格納用配列
	var $load_flg	= false;	//ロガー設定情報読込みフラグ
	var $varsion	= "1.0";
	var $update = "2012.12.19";
	var $memo = "にかほ市専用クラスから改変し田村市";
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
	var $EditSys	= array();							//編集可能なデータ帳票
	var $ColSys = array("100","200","300","400");	//手動収集可能なデータ帳票
	var $DelSys = array("100","200","300","400");	//削除可能なデータ帳票

	var $flg_edit_rainrui = true;//累計雨量編集
	var $flg_edit_weather = false;//天気編集
	var $flg_edit_warning = false;//気象庁編集
	var $flg_edit_alert	 = true;//閾値編集

	//帳票毎でこのロガーが出力できる項目を定義
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