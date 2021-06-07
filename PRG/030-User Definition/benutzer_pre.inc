<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : benutzer_pre.inc                                             //
// Version     : 21.1                                                         //
// Begin       : 2020-10-20                                                   //
// Last Change : 2020-10-22                                                   //
// Author      : FeRox Management Consulting GmbH & Co. KG                    //
//               Adolf-Langer-Weg 11a, D-94036 Passau (Germany)               //
//               https://www.ferox.de - info@ferox.de                         //
// License     : GNU-GPL v3 (https://opensource.org/licenses/GPL-3.0)         //
// -------------------------------------------------------------------------- //
// fx-project - An open source PHP Project Managament Software                //
// Copyright  © FeRox Management Consulting GmbH & Co. KG                     //
// -------------------------------------------------------------------------- //
// This program is free software: you can redistribute it and/or modify       //
// it under the terms of the GNU General Public License as published by       //
// the Free Software Foundation, either version 3 of the License, or          //
// (at your option) any later version.                                        //
//                                                                            //
// This program is distributed in the hope that it will be useful,            //
// but WITHOUT ANY WARRANTY; without even the implied warranty of             //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              //
// GNU General Public License for more details.                               //
//                                                                            //
// You should have received a copy of the GNU General Public License          //
// along with this program.  If not, see <https://www.gnu.org/licenses/>.     //
//                                                                            //
// See ../LICENSE.TXT file for more information.                              //
// -------------------------------------------------------------------------- //
// LICENSING ADDENDUM:                                                        //
// Programs in the SPP (Special Programs) subfolder are coded extensions of   //
// the open source software fx-project. These programs are offered for sale   //
// by the manufacturer FeRox Management Consulting GmbH & Co. KG and require  //
// a valid key for execution. It is forbidden to resell these programs        //
// and/or keys or to pass them on free of charge or use them without the      //
// express written permission of FeRox Management Consulting GmbH & Co. KG.   //
////////////////////////////////////////////////////////////////////////////////

/**
 * @file
 * PF 30: User Definition
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

if($Benutzer_ID || ($GLOBALS['fxpglobals']['action'] == FXP_CREATE))
{
	// Muss-Felder
	$Name_Benutzer_muss=true;

	// Filter
	$flt=filter_person('and', true, false, 'benutzerpersid');
	$maske19_flt=$mskfilter;

	// zugeh. Person:
	if(!fxIsArray($_POST) || !isset($_POST['benutzerpersid']) || ($_POST['benutzerpersid'] < 1))
		$_POST['benutzerpersid']=$GLOBALS['fxpglobals']['person'];
	$benutzerpersid_ds=0;
	$benutzerpersid_arg=array('modus'=>2, 'ptpa'=>array('persontypes'=>array(FXP_PT_CLIENT_CP,FXP_PT_CUSTOMER_CP,FXP_PT_PARTNER_CP,FXP_PT_EMPLOYEE,FXP_PT_CONTRACTOR,FXP_PT_CONTACT_CP), 'all_clients'=>false, 'noclient'=>true, 'with_user'=>true, 'filter'=>$flt));
	$benutzerpersid_array=auswahl_Personen_ID(1, array(), 'benutzerpersid');
//fxDebug($benutzerpersid_array,'$benutzerpersid_array', 0);

	// Neuanlage separat
	if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
	{
		unset($maskenfeldwerte);
		$maskenfeldwerte['benutzerid']='~<span class="grey">'.id_max('Benutzer', 'Benutzer_ID', '', '', 1, false).'</span>';	// Wert nur zur Anzeige holen

		$maskenfeldwerte['benutzerpersid']=$_POST['benutzerpersid'];
		if($err || $val_err)
		{
			if(strlen($_POST['gueltig_von']) && (strlen($_POST['gueltig_von']) != 14))
				$_POST['gueltig_von']=validate_entry($_POST['gueltig_von'], $derr, 'datum');
			if(strlen($_POST['gueltig_bis']) && (strlen($_POST['gueltig_bis']) != 14))
				$_POST['gueltig_bis']=validate_entry($_POST['gueltig_bis'], $derr, 'datum');

			$maskenfeldwerte['name_benutzer']	= $_POST['Name_Benutzer'];

			$maskenfeldwerte['e_mail_adresse']	= $_POST['E_Mail_Adresse'];
			$maskenfeldwerte['gueltig_von']		= $_POST['gueltig_von'];
			$maskenfeldwerte['passwort_alias']	= $_POST['Passwort_alias'];

			$maskenfeldwerte['b_admin']			= $_POST['b_admin'];
			$maskenfeldwerte['gueltig_bis']		= $_POST['gueltig_bis'];
			$maskenfeldwerte['passwort_check']	= $_POST['Passwort_check'];

			$maskenfeldwerte['sprache']			= $_POST['Sprache'];

			$maskenfeldwerte['kosten_benutzer']	= $_POST['Kosten_Benutzer'];
			$maskenfeldwerte['benutzergr_id']	= $_POST['Benutzergr_ID'];
		}
		else
		{

			$maskenfeldwerte['kosten_benutzer']	= 2160;	// Benutzer darf Kosten sehen entspr. seiner Mitarbeiter-Rolle
			$maskenfeldwerte['sprache']			= $GLOBALS['fxpglobals']['lang'];
		}

		$Sprache_ds=0;
		$Kosten_Benutzer_ds=0;
		$Button_Reset_nv=true;
	}

	// Anzeigen, Ändern + Löschen
	else if($Benutzer_ID)
	{
		$sql  = "SELECT name_benutzer, benutzer_id AS benutzerid, personen_id AS benutzerpersid, e_mail_adresse, kosten AS kosten_benutzer, b_admin, gueltig_von, gueltig_bis";
		$sql .= " FROM benutzer WHERE benutzer_id=".$Benutzer_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$maskenfeldwerte=db_values($sql);
		$maskenfeldwerte['benutzerid']='~'.$maskenfeldwerte['benutzerid'];
		if(!$maskenfeldwerte['kosten_benutzer'])
			$maskenfeldwerte['kosten_benutzer']=2160;	// Standard
		if(($GLOBALS['fxpglobals']['action'] != FXP_CHANGE) && strlen($maskenfeldwerte['e_mail_adresse']))
			$maskenfeldwerte['e_mail_adresse']='~<a href="mailto:'.$maskenfeldwerte['e_mail_adresse'].'"><img class="op50" src="'.$GLOBALS['gfxpath'].'eml_b_20x20.png" height="15" align="top">&nbsp;'.$maskenfeldwerte['e_mail_adresse'].'</a>';
		if(($GLOBALS['fxpglobals']['action'] != FXP_CHANGE) && $maskenfeldwerte['benutzerpersid'] && isset($benutzerpersid_array[$maskenfeldwerte['benutzerpersid']]))
		{
			$psa=selGetStyle($benutzerpersid_array[$maskenfeldwerte['benutzerpersid']]);
//fxDebug($psa,'$psa', 0);
			while(substr($psa['tx'],0,6) == '&nbsp;')
				$psa['tx']=substr($psa['tx'],6);
			$sbp=strpos($psa['tx'],'[');
			if($sbp)
				$psa['tx']=substr($psa['tx'],0,$sbp);
			$tt='';
			if(strlen($psa['tt']))
				$tt=' tooltip="'.$psa['tt'].'" style="cursor:help;"';
			$maskenfeldwerte['benutzerpersid']='~<span'.$tt.'>'.trim($psa['tx']).'</span>';
		}

		// Felder ausblenden
		$Benutzergr_ID_nv=true;
		$Dummy3_nv=true;
		$Sprache_nv=true;
		if($GLOBALS['fxpglobals']['action'] != FXP_CHANGE)
		{
			$Passwort_nv=true;
			$Passwort_alias_nv=true;
			$Passwort_check_nv=true;
		}
		else
		{
			if($err || $val_err)
			{
				if(strlen($_POST['gueltig_von']) && (strlen($_POST['gueltig_von']) != 14))
					$_POST['gueltig_von']=validate_entry($_POST['gueltig_von'], $derr, 'datum');
				if(strlen($_POST['gueltig_bis']) && (strlen($_POST['gueltig_bis']) != 14))
					$_POST['gueltig_bis']=validate_entry($_POST['gueltig_bis'], $derr, 'datum');

				$maskenfeldwerte['name_benutzer']	= $_POST['Name_Benutzer'];

				$maskenfeldwerte['e_mail_adresse']	= $_POST['E_Mail_Adresse'];
				$maskenfeldwerte['gueltig_von']		= $_POST['gueltig_von'];
				$maskenfeldwerte['passwort_alias']	= $_POST['Passwort_alias'];

				$maskenfeldwerte['b_admin']			= $_POST['b_admin'];
				$maskenfeldwerte['gueltig_bis']		= $_POST['gueltig_bis'];
				$maskenfeldwerte['passwort_check']	= $_POST['Passwort_check'];

				$maskenfeldwerte['benutzerpersid']	= $_POST['benutzerpersid'];

				$maskenfeldwerte['kosten_benutzer']	= $_POST['Kosten_Benutzer'];
			}
			$Kosten_Benutzer_ds=0;
		}

		// Button ausblenden
		if($GLOBALS['fxpglobals']['action'] != FXP_CHANGE)
			$Button_Reset_nv=true;
		if($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY)
			$Button_Multi_nv=true;
	}
}
?>