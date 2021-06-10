<?
//==========================================================//
//	CX1000ロガークラス										//
//----------------------------------------------------------//
//	処理概要												//
//	　CX1000用												//
//	　越谷市風向風速局用クラス								//
//----------------------------------------------------------//
//	変更履歴												//
//		2020/07/08	N.ARAI									//
//		・新規作成											//
//==========================================================//
class LoggerCX1000V2_WIND extends LoggerCX1000V2
{
	//変数宣言
	var $varsion	= "1.0";
	var $update		= "2020.07.08";
	var $memo		= "";
	var $id			= "CX1000V2_WIND";
	var $type		= "1";	//ロガータイプ　1 => 気象ロガー	 2 => 雨量ロガー

	var $SettingTemplate = "lgr_cx1000v2.tmpl";

	var $cx1000_input_rain_mode = false;// ロガー設定項目に雨量入力切替の有無（CX1000限定）
	var $cx1000_input_lgr_snow = false;	// ロガー設定項目に積雪入力切替の有無（CX1000限定）

	var $EditSys	= array("100","200");	//編集可能なデータ帳票
	var $ColSys		= array("100");			//手動収集可能なデータ帳票
	var $DelSys		= array("100","200");	//削除可能なデータ帳票

	var $Recal1minFlg	= true;				//1分データを使用しての計算が可能／不可能
	var $RecalDisregard	= false;			//1分データチェックを無視して処理をするかの可／否

	//帳票毎でこのロガーが出力できる項目を定義
	var $OutField = array(
		"000" => array(
			"avg_ws",
			"avg_wd",
			"max_ws",
			"max_wd"
		),
		"100" => array(
			"avg_ws",
			"avg_wd",
			"max_ws",
			"max_wd"
		),
		"200" => array(
			"avg_ws",
			"avg_wd",
			"max_ws",
			"max_wd"
		),
		"300" => array(
			"avg_ws",
			"avg_wd",
			"max_ws",
			"max_wd"
		),
		"400" => array(
			"avg_ws",
			"avg_wd",
			"max_ws",
			"max_wd"
		)
	);

	//帳票毎でこのロガーが編集できる項目を定義
	var $EditField = array(
		"100" => array(
			"avg_ws",
			"avg_wd",
			"max_ws",
			"max_wd"
		),
		"200" => array(
			"avg_ws",
			"avg_wd",
			"max_ws",
			"max_wd"
		)
	);
}
?>