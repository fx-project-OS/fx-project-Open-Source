<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : personen_kompakt_man_db.inc                                  //
// Version     : 21.2                                                         //
// Begin       : 2020-10-05                                                   //
// Last Change : 2020-10-05                                                   //
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
 * PF 166: Client database action preparations
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

// Don't display database errors
$db_noerror_old=$db_noerror;
$db_noerror=true;

// Use new client number in creation mode
if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
	$_cid=$_ncid;

// Name
$db_Pname="'".convert_string($Pname, 'todb')."'";

// Name affix
if(strlen($Name_Zusatz))
	$db_Name_Zusatz="'".convert_string($Name_Zusatz, 'todb')."'";
else
	$db_Name_Zusatz="NULL";

// Identification
if(strlen($Personen_Initialen))
	$db_Personen_Initialen="'".convert_string($Personen_Initialen, 'todb')."'";
else
	$db_Personen_Initialen="NULL";

// Validity fields
$db_gueltig_von="NULL";
if(strlen($_POST['gueltig_von']))
	$db_gueltig_von="'".substr($_POST['gueltig_von'], 0, 8)."000000'";
$db_gueltig_bis="NULL";
if(strlen($_POST['gueltig_bis']))
	$db_gueltig_bis="'".substr($_POST['gueltig_bis'], 0, 8)."240000'";

// Inactive
if($inaktiv)
	$db_inaktiv='1';
else
	$db_inaktiv='0';

// User Name
$db_Name_Benutzer="'".convert_string(strtolower($Name_Benutzer), 'todb')."'";

// Email Address
if(strlen($E_Mail_Adresse))
	$db_E_Mail_Adresse="'".convert_string($E_Mail_Adresse, 'todb')."'";
else
	$db_E_Mail_Adresse="NULL";

// Password(s)
if(($Passwort_alias == 'leer') || ($Passwort_alias == 'empty') || ($Passwort_alias == '---'))
	$Passwort_alias='';
$db_Passwort_alias="'".fxf_ecPasswd($Passwort_alias, '', $_cid, 1)."'";

// Prepare client sql array
$dbz_arr['man']=array();
$dbz=0;

// Datetime
$db_zeitstempel="'".$GLOBALS['datetime']."'";


// Create
if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
{
	// Create new client by copying and adjusting selected source client
	if($_scid && fxIsArray($GLOBALS['fxpglobals']['dbparam']['ctables']))
	{
		// Don't copy the following tables
		$tab_ignore=array(
			'd_dok_transfer_log'=>true,	'd_dok_verlauf'=>true, 'd_dokumente'=>true,	// DMS
			'felder'=>true,															// Field generator fields
			'log_sync'=>true,														// Outlook synchronisations
			'masken'=>true															// Mask generator fields
		);

		// Get client and client's admin ids
		$_pid=(int)db_value("SELECT personen_id FROM personen WHERE personentyp=".FXP_PT_CLIENT." AND mandanten_id=".$_scid);
		$_apid=(int)db_value("SELECT personen_id FROM personen WHERE personentyp=".FXP_PT_CLIENT_CP." AND mandanten_id=".$_scid." ORDER BY personen_id");

		// Copy all client tables with new client id
		foreach($GLOBALS['fxpglobals']['dbparam']['ctables'] as $tabname => $tabtype)
		{
			// Table exists
			if(!isset($tab_ignore[$tabname]) && db_check($tabname, 't'))
			{
				$tid=0;
				$hid=0;
				$tca=array();

				// Sql condition
				$cond=" FROM ".$tabname." WHERE mandanten_id=".$_scid;
				// Ignore mask generator program functions in tables "rechte_trans" and "teinstellung"...
				if(($tabname == 'rechte_trans') || ($tabname == 'teinstellung'))
					$cond .= " AND trans_id>0";
				// ...and in table "wertetab"
				else if($tabname == 'wertetab')
					$cond .= " AND wertetabid>0";

				// Check if table has entries
				$sql="SELECT COUNT(mandanten_id)".$cond;
				$cnt=(int)db_value($sql);
				if($cnt > 0)
				{
fxDebug($cnt,$sql, 3);
					// Get table id
					$sql="SELECT id_tabellen FROM tabellen WHERE id_tabellen>0 AND ".db_like('tabname', $tabname);
					$tid=(int)db_value($sql);
fxDebug($tid,$sql, 3);
				}

				// Get history table id and columns
				if($tid)
				{
					$sql="SELECT id_tabellen FROM tabellen WHERE ".db_like('tabname', $tabname.'_history');
					$hid=(int)db_value($sql);
fxDebug($hid,$sql, 3);

					$sql="SELECT f.id, f.feldname FROM tab_felder t, felder f WHERE t.idintabellen=".$tid." AND f.id=t.idinfelder AND f.mandanten_id IN (0,".$_scid.") ORDER BY t.positionbeitabelle";
					$tca=db_values($sql, '*');
fxDebug($tca,$sql, 3);
				}

				// Copy table entries with new client id
				if(fxIsArray($tca))
				{
					$cs='';
					$vs='';
					foreach($tca as $tc)
					{
						$fieldname=strtolower($tc['feldname']);

						if(strlen($cs))
							$cs .= ",";
						$cs .= $fieldname;

						if(strlen($vs))
							$vs .= ",";
						if(($fieldname == 'mandanten_id') || ($fieldname == 'zuordnungs_mid'))
							$vs .= $_cid." AS ".$fieldname;
						else if(($fieldname == 'historie') && $hid)
							$vs .= "0 AS ".$fieldname;
						else
							$vs .= $fieldname;
					}
					$sql="INSERT INTO ".$tabname." (".$cs.") SELECT ".$vs.$cond;
					$dbz_arr['man'][$dbz++]=$sql;
				}
			}
		}

		// Change client in table "personen"
		$sql  = "UPDATE personen SET pname=".$db_Pname.", name_zusatz=".$db_Name_Zusatz.", personen_initialen=".$db_Personen_Initialen.", gueltig_von=".$db_gueltig_von.", gueltig_bis=".$db_gueltig_bis.", inaktiv=".$db_inaktiv;
		$sql .= " WHERE personen_id=".$_pid." AND mandanten_id=".$_cid;
		$dbz_arr['man'][$dbz++]=$sql;

		// Change client's administrator in table "personen"
		$sql="UPDATE personen SET name_zusatz=".$db_Pname.", inaktiv=0 WHERE personen_id=".$_apid." AND mandanten_id=".$_cid;
		$dbz_arr['man'][$dbz++]=$sql;

		// Change client's administrator in table "benutzer"
		$sql  = "UPDATE benutzer SET name_benutzer=".$db_Name_Benutzer.", e_mail_adresse=".$db_E_Mail_Adresse.", passwort=".$db_Passwort_alias.", login_zaehler=0";
		$sql .= " WHERE benutzer_id=1 AND mandanten_id=".$_cid;
		$dbz_arr['man'][$dbz++]=$sql;

		// Change client user datas in table "benutzer"
		$sql="SELECT benutzer_id, name_benutzer FROM benutzer WHERE mandanten_id=".$_cid." AND benutzer_id>1";
		$tmp=db_values($sql, '*');
fxDebug($tmp, $sql, 3);
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
			{
				$uid=(int)$t['benutzer_id'];
				$upw=fxf_ecPasswd($Passwort_alias, '', $_cid, $uid);

				$deact="";
				if(isset($deact_user[$uid]))
					$deact=", inaktiv=".FXP_USER_INACTIVE;

				$sql="UPDATE benutzer SET passwort='".$upw."', login_zaehler=0";
				if(isset($deact_user[$uid]))
					$sql .= ", inaktiv=".FXP_USER_INACTIVE;
				$sql .= " WHERE benutzer_id=".$uid." AND mandanten_id=".$_cid;
				$dbz_arr['man'][$dbz++]=$sql;
			}
		}
	}

	// Create new empty client
	else
	{
		// Create client in table "personen"
		$_pid=id_max('Personen', 'Personen_ID');
		$sql  = "INSERT INTO personen";
		$sql .= " (mandanten_id, personen_id, personentyp, pname, vorname, personen_initialen, name_zusatz, anrede, perstitel, pposition, elter, geschlecht, abt_id, vertragsstandort, einsatzort, pzaehler, gueltig_von, gueltig_bis, inaktiv, historie, transid, aenderungs_id, zeitstempel) ";
		$sql .= "VALUES";
		$sql .= " (".$_cid.", ".$_pid.", ".FXP_PT_CLIENT.", ".$db_Pname.", NULL, ".$db_Personen_Initialen.", ".$db_Name_Zusatz.", 0, 0, 0, 0, 0, 0, 0, 0, 0, ".$db_gueltig_von.", ".$db_gueltig_bis.", ".$db_inaktiv.", 0, ".(int)$GLOBALS['fxpglobals']['tr'].", ".$GLOBALS['fxpglobals']['person'].", ".$db_zeitstempel.")";
		$dbz_arr['man'][$dbz++]=$sql;

		// Create client's administrator as client's contact person in table "personen"
		$_apid=id_max('Personen', 'Personen_ID');
		$sql  = "INSERT INTO personen";
		$sql .= " (mandanten_id, personen_id, personentyp, pname, vorname, personen_initialen, name_zusatz, anrede, perstitel, pposition, elter, geschlecht, abt_id, vertragsstandort, einsatzort, pzaehler, gueltig_von, gueltig_bis, inaktiv, historie, transid, aenderungs_id, zeitstempel) ";
		$sql .= "VALUES";
		$sql .= " (".$_cid.", ".$_apid.", ".FXP_PT_CLIENT_CP.", 'Administrator', NULL, NULL, ".$db_Pname.", 2803, 0, 924, ".$_pid.", ".FXP_GT_DIVERSE.", 0, 0, 0, 0, NULL, NULL, 0, 0, ".(int)$GLOBALS['fxpglobals']['tr'].", ".$GLOBALS['fxpglobals']['person'].", ".$db_zeitstempel.")";
		$dbz_arr['man'][$dbz++]=$sql;

		// Create user #1 for the client's administrator in table "benutzer"
		$sql  = "INSERT INTO benutzer";
		$sql .= " (mandanten_id, benutzer_id, name_benutzer, personen_id, e_mail_adresse, passwort, b_admin, kosten, login_zaehler, status_benutzer, transid, aenderungs_id, zeitstempel) ";
		$sql .= "VALUES";
		$sql .= " (".$_cid.", 1, ".$db_Name_Benutzer.", ".$_apid.", ".$db_E_Mail_Adresse.", ".$db_Passwort_alias.", 1, 2160, 0, 0, ".(int)$GLOBALS['fxpglobals']['tr'].", ".$GLOBALS['fxpglobals']['person'].", ".$db_zeitstempel.")";
		$dbz_arr['man'][$dbz++]=$sql;

		// Create default profiles
		$pcalls=fxf_getDefaultProfiles($_cid);
fxDebug($pcalls,'$pcalls', 3);
		if(fxIsArray($pcalls))
		{
			foreach($pcalls as $sql)
				$dbz_arr['man'][$dbz++]=$sql;
		}
	}
//$err=true;
}

// Change
else if(($GLOBALS['fxpglobals']['action'] == FXP_CHANGE) && $_pid && $_cid)
{
	// Change client in table "personen"
	$sql="UPDATE personen SET pname=".$db_Pname.", name_zusatz=".$db_Name_Zusatz.", personen_initialen=".$db_Personen_Initialen;
	if($_cid != $GLOBALS['fxpglobals']['client'])
		$sql .= ", gueltig_von=".$db_gueltig_von.", gueltig_bis=".$db_gueltig_bis.", inaktiv=".$db_inaktiv;
	$sql .= " WHERE personen_id=".$_pid." AND mandanten_id=".$_cid;
	$dbz_arr['man'][$dbz++]=$sql;

	// Change client's administrator in table "personen"
	$sql="SELECT personen_id FROM personen WHERE mandanten_id=".$_cid." AND personentyp=".FXP_PT_CLIENT_CP." ORDER BY personen_id";
	$_apid=(int)db_value($sql);
fxDebug($_apid,$sql, 3);
	if($_apid)
	{
		$sql="UPDATE personen SET name_zusatz=".$db_Pname.", inaktiv=0 WHERE personen_id=".$_apid." AND mandanten_id=".$_cid;
		$dbz_arr['man'][$dbz++]=$sql;

		// Change client's administrator in table "benutzer"
		$sql="UPDATE benutzer SET name_benutzer=".$db_Name_Benutzer.", e_mail_adresse=".$db_E_Mail_Adresse;
		if(strlen($Passwort_check) && strlen($db_Passwort_alias))
			$sql .= ", passwort=".$db_Passwort_alias;
		$sql .= " WHERE benutzer_id=1 AND mandanten_id=".$_cid;
		$dbz_arr['man'][$dbz++]=$sql;
	}
}

// Delete
else if(($GLOBALS['fxpglobals']['action'] == FXP_DELETE) && $_pid && $_cid && $_POST['Button_PGLoeschen'])
{
	// Delete client sequences
	$csa=fxf_dbSeqDelete('m'.$_cid, '', '', '', true, false);
	if(sizeof($csa))
	{
		foreach($csa as $sql)
			$dbz_arr['man'][$dbz++]=$sql;
	}

	// Get all client tables and delete client datas
	$cta=get_all_tables(true, true);
fxDebug($cta, 'All client tables', 3);
	if(fxIsArray($cta))
	{
		foreach($cta as $tabname => $orig_tabname)
		{
			if((substr($tabname, 0, 1) == '§') || ($tabname == 'arte'))
				continue;

			// Does this table really exist?
			if(!db_check($tabname, 't'))
				continue;

			$sqladd="";
			if($tabname == 'zeitdaten')
				$sqladd=" OR zuordnungs_mid=".$_cid;
			$check="SELECT mandanten_id FROM ".$tabname." WHERE mandanten_id=".$_cid.$sqladd;
			$datafound=db_value($check);
			if($datafound)
			{
				$sql="DELETE FROM ".$tabname." WHERE mandanten_id=".$_cid.$sqladd;
				$dbz_arr['man'][$dbz++]=$sql;
			}
		}
	}

	// Execute additional sql's?
	if($dbz > 1)
	{
		// Delete formula-generator's masks
		$mndel='';
		$sql="SELECT nummer, name_maske FROM masken WHERE nummer<0 AND mandanten_id=".$_cid;
		$fgarr=db_values($sql, '*');
fxDebug($fgarr, 'All FG masks: $sql='.$sql, 3);
		if(fxIsArray($fgarr))
		{
			foreach($fgarr as $fgval)
			{
				if(strlen($mndel))
					$mndel .= ", ";
				$mndel .= $fgval['nummer'];
			}
		}
		if(strlen($mndel))
		{
			$tadel=array('masken'=>'nummer', 'maskenfelder'=>'nummer');
			foreach($tadel as $dnr => $dname)
			{
				$sql="DELETE FROM ".$dname." WHERE ".$dname." IN (".$mndel.")";
				$dbz_arr['man'][$dbz++]=$sql;
			}
		}

		// Delete formula-generator's tables
		$tndel='';
		$sql="SELECT id_tabellen, tabname FROM tabellen WHERE id_tabellen<0 AND ".db_like('tabname', 'fg_%_'.$_cid);
		$tbarr=db_values($sql, '*');
fxDebug($tbarr, 'All FG tables: $sql='.$sql, 3);
		if(fxIsArray($tbarr))
		{
			foreach($tbarr as $tbval)
			{
				if(strlen($tndel))
					$tndel .= ", ";
				$tndel .= $tbval['id_tabellen'];
			}
		}
		if(strlen($tndel))
		{
			$tadel=array('tabellen'=>'id_tabellen', 'trans'=>'transakt_id', 'trans_aktionen'=>'trans_id', 'trans_maske'=>'transakt_id', 'menu'=>'trans_id');
			foreach($tadel as $dnr => $dname)
			{
				$sql="DELETE FROM ".$dname." WHERE ".$dname." IN (".$tndel.")";
				$dbz_arr['man'][$dbz++]=$sql;
			}
		}

		// Reset client id
		$iddelarr=fxf_dbSeqDelete('', 'personen', 'mandanten_id', '', true, false);
		if(sizeof($iddelarr))
		{
			foreach($iddelarr as $iddel)
				$dbz_arr['man'][$dbz++]=$iddel;
		}
	}
}

// Reset error messages
if(!$db_noerror_old)
	unset($db_noerror);
?>