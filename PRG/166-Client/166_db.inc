<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : 166_db.inc                                                   //
// Version     : 21.2                                                         //
// Begin       : 2020-10-06                                                   //
// Last Change : 2020-10-06                                                   //
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
 * PF 166: Client database actions
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$err_man=false;
$err_pers=false;
$err_adr=false;
$err_kom=false;

$_bpid=$_pid;
//fxDebug($dbz_arr, 'All SQL calls', 0);

// Get person data
if($GLOBALS['fxpglobals']['action'] == FXP_CHANGE)
	$hdat=getPersonData($_bpid);
else
	$hdat=array();

// Execute all client sql calls
if(!$err && fxIsArray($dbz_arr['man']))
{
fxDebug($dbz_arr['man'], 'All client sql calls', 3);
	if(($GLOBALS['fxpglobals']['action'] == FXP_DELETE) && (sizeof($dbz_arr['man']) == 1))
		$err_man=true;
	else
	{
		foreach($dbz_arr['man'] as $dbcall)
		{
			$suc=db_query($dbcall);
fxDebug($suc,$dbcall, 2);
			if(!$suc)
			{
				$err_man=true;
				break;
			}
		}
	}

	// Erfolgs- bzw. Fehlermeldungen ausgeben
	if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
	{
		if($err_man)
		{
			$_pid=0;
			error_msg(120, false, $_cid);	// 120: Fehler beim Anlegen des neuen Mandanten $var1!
		}
		else
		{
			error_msg(1350, false, $_cid);	// 1350: Mandant Nummer $var1 wurde erfolgreich angelegt.

			// Create new client directory by copying selected source client directory files
			if($_scid && fxIsArray($GLOBALS['fxpglobals']['dbparam']['ctables']))
			{
				$_dcdn=$GLOBALS['datpath'].$_cid.'/';
				$derr=fxCreatePath($_dcdn);
				if(!strlen($derr))
				{
					$_dcdl=strlen($_dcdn);
					$_scdn=$GLOBALS['datpath'].$_scid.'/';
					$_scfa=fxScanDir($_scdn, false, true, '/^.+\.(cfg|css|gif|jpg|png)$/i', true);
fxDebug($_scfa,'$_scfa: $_scdn='.$_scdn, 3);
					if($_scfa['file_entries'] > 0)
					{
						foreach($_scfa['files'] as $sfn => $sfp)
						{
							$dfp=$_dcdn.substr($sfp,$_dcdl);
							$derr=fxCreatePath($dfp);
							if(!strlen($derr))
								$success=copy($sfp, $dfp);
						}
					}
				}
			}
		}
	}
	else if($GLOBALS['fxpglobals']['action'] == FXP_CHANGE)
	{
		if($err_man)
			error_msg(129, false, $_cid);	// 129: Fehler beim Ändern der Mandanten-Daten von Mandant $var1!
		else
			error_msg(130, false, $_cid);	// 130: Die Mandanten-Daten von Mandant $var1 wurden erfolgreich geändert.
	}
	else if($GLOBALS['fxpglobals']['action'] == FXP_DELETE)
	{
		if($err_man)
		{
			error_msg(121, false, $_cid);	// 121: Fehler beim Löschen des Mandanten $var1!
			$Button_Loeschen=true;
		}
		else
		{
			error_msg(122, false, $_cid);	// 122: Der Mandant $var1 wurde erfolgreich gelöscht.

			$maske387_nv=true;
			$maske394_nv=true;
			$maske384_nv=true;
			$maske386_nv=true;
			$maske388_nv=true;

			// Drop formula generator tables
			if(fxIsArray($tbarr))
			{
				foreach($tbarr as $tbval)
				{
					$sql="DROP TABLE ".$tbval['tabname'].$GLOBALS['fxpglobals']['dbparam']['dbcascade'];
					$success=db_query($sql);
fxDebug($success, $sql, 2);
				}
			}

			// Delete client directory
			$_cdn=$GLOBALS['datpath'].$_cid;
			$success=fxDelete($_cdn, '', 0, true);

			// Delete client documents
			$_cdn=$GLOBALS['fxpglobals']['settings']['dok_archiv'].$_cid;
			$success=fxDelete($_cdn, '', 0, true);
		}
	}
}
if($err_man)
	$err=true;

// Client $var1 doesn't exist anymore!
if(!$err && !fxIsArray($dbz_arr['man']))
{
	$ex_mand=db_value("SELECT personen_id FROM personen WHERE mandanten_id=".$_cid." AND personentyp=274");
	if(!$ex_mand)
	{
		error_msg(288, false, $_cid);
		$err=true;
	}
}

// Execute all person sql calls?
if(!$err && fxIsArray($dbz_arr['pers']))
{
fxDebug($dbz_arr['pers'], 'All person sql calls', 3);
	foreach($dbz_arr['pers'] as $dbcall)
	{
		if(fxIsArray($dbcall['ckd']))	// Delete with check if data is not used in other tables
			$err_pers=db_check_delete($dbcall['ckd']['tab'], $dbcall['ckd']['fld'], $dbcall['ckd']['val'], $dbcall['sql']);
		else
			$err_pers=db_x($dbcall['sql']);
fxDebug($err_pers,$dbcall['sql'], 2);
		if($err_pers)
			break;
	}

	// Adjust customer
	if(!$err_pers && ($cust_project || $cust_order))
	{
		if(!$err_pers && $cust_project)
		{
			$sql="UPDATE projekte SET kunde=".$cust_project." WHERE ansprechpartner=".$_pid." AND mandanten_id=".$_cid;
			$err_pers=db_x($sql);
fxDebug($err_pers,$sql, 2);
		}
		if(!$err_pers && $cust_order)
		{
			$sql="UPDATE auftrag SET kunde=".$cust_order." WHERE ansprechpartner=".$_pid." AND mandanten_id=".$_cid;
			$err_pers=db_x($sql);
fxDebug($err_pers,$sql, 2);
		}

		// Error while changing the according projects of the contact person to the new customer!
		if($err_pers)
			error_msg(1992);
		// The contact person was successfully assigned to the new customer. All projects were changed accordingly.
		else
			error_msg(1991);
	}

	// Change contact to customer?
	if(!$err_pers && ($_ptype == FXP_PT_CONTACT) && $_POST['Kunde'])
	{
		$sql="UPDATE personen SET personentyp=".FXP_PT_CUSTOMER.", elter=0 WHERE personen_id=".$_pid." AND mandanten_id=".$_cid;
		$err_pers=db_x($sql);
fxDebug($err_pers,$sql, 2);

		if(!$err_pers)
		{
			$sql="UPDATE personen SET personentyp=".FXP_PT_CUSTOMER_CP." WHERE elter=".$_pid." AND mandanten_id=".$_cid;
			$err_pers=db_x($sql);
fxDebug($err_pers,$sql, 2);
		}

		// The contact was successfully switched to the new type: <b>Customer</b>!
		if(!$err_pers)
			error_msg(1993);
	}

	// Error/Success message
	$fmpname='<b>&quot;'.$Pname;
	if(strlen($Vorname))
		$fmpname .= ', '.$Vorname;
	$fmpname .= '&quot;</b> <i>('.meldung($_ptype, false).')</i> ';
	if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
	{
		$_pid=0;
		if($err_pers)
			error_msg(149, false, $fmpname);	// 149: Fehler beim Anlegen von $var1!
		else
			error_msg(144, false, $fmpname);	// 144: $var1 wurde erfolgreich angelegt.
	}
	else if($GLOBALS['fxpglobals']['action'] == FXP_CHANGE)
	{
		if($err_pers)
			error_msg(150, false, $fmpname);	// 150: Fehler beim Ändern von $var1!
		else
			error_msg(145, false, $fmpname);	// 145: $var1 wurde erfolgreich geändert.
	}
	else if($GLOBALS['fxpglobals']['action'] == FXP_DELETE)
	{
		if($err_pers)
		{
			error_msg(151, false, $fmpname);	// 151: Fehler beim Löschen von $var1!
			$GLOBALS['fxptdata']['backup']['warnhinweis']=true;
		}
		else
		{
			$_pid=0;
			error_msg(146, false, $fmpname);	// 146: $var1 wurde erfolgreich gelöscht.
			$maske384_nv=true;
			$maske386_nv=true;
		}
	}
}
if($err_pers)
	$err=true;

// Execute address sql calls?
$da=0;
if(!$err)
{
	if(fxIsArray($dbz_arr['adr']))
	{
		$dbins=0;
		$dbupd=0;
		$dbdel=0;
fxDebug($dbz_arr['adr'], 'All address sql calls', 3);
		foreach($dbz_arr['adr'] as $dbcall)
		{
			if(fxIsArray($dbcall['ckd']))
				$err_adr=db_check_delete($dbcall['ckd']['tab'], $dbcall['ckd']['fld'], $dbcall['ckd']['val'], $dbcall['sql']);
			else
				$err_adr=db_x($dbcall['sql']);
fxDebug($err_adr,$dbcall['sql'], 2);
			if($err_adr)
				break;
			else if($dbcall['typ'] == 'insert')
				$dbins++;
			else if($dbcall['typ'] == 'update')
				$dbupd++;
			else
			{
				$da=2;
				$dbdel++;
			}
		}

		// Error/Success message
		if($err_adr)
		{
			if($GLOBALS['fxpglobals']['action'] != FXP_DELETE)
				error_msg(131);	// 131: Fehler beim Ändern bzw. Neuanlage der Adressen!
			else if($dbdel)
				error_msg(132);	// 132: Fehler beim Löschen der Adressen!
		}
		else
		{
			if($dbins)
				error_msg(133, false, $dbins);	// 133: Es wurde(n) $var1 Adresse(n) erfolgreich hinzugefügt.
			if($dbupd)
				error_msg(134, false, $dbupd);	// 134: Es wurde(n) $var1 Adresse(n) erfolgreich geändert.
			if($dbdel)
				error_msg(135, false, $dbdel);	// 135: Es wurde(n) $var1 Adresse(n) erfolgreich gelöscht.
		}
	}
	else if(($GLOBALS['fxpglobals']['action'] == FXP_DELETE) && $_POST['Button_PZLoeschen'] && isset($_POST['Adressen_ID']))
		$da=1;
}
if($err_adr)
	$err=true;

// Execute communication sql calls?
$dk=0;
if(!$err)
{
	if(fxIsArray($dbz_arr['kom']))
	{
		$dbins=0;
		$dbupd=0;
		$dbdel=0;
fxDebug($dbz_arr['kom'], 'All communication sql calls', 3);
		foreach($dbz_arr['kom'] as $dbcall)
		{
			if(fxIsArray($dbcall['ckd']))
				$err_kom=db_check_delete($dbcall['ckd']['tab'], $dbcall['ckd']['fld'], $dbcall['ckd']['val'], $dbcall['sql']);
			else
				$err_kom=db_x($dbcall['sql']);
fxDebug($err_kom,$dbcall['sql'], 2);
			if($err_kom)
				break;
			else if($dbcall['typ'] == 'insert')
				$dbins++;
			else if($dbcall['typ'] == 'update')
				$dbupd++;
			else
			{
				$dk=2;
				$dbdel++;
			}
		}

		// Error/Success message
		if($err_kom)
		{
			if($GLOBALS['fxpglobals']['action'] != FXP_DELETE)
				error_msg(136);	// 136: Fehler beim Ändern bzw. Neuanlage der Kommunikationsdaten!
			else
				error_msg(137);	// 137: Fehler beim Löschen der Kommunikationsdaten!
		}
		else
		{
			if($dbins)
				error_msg(138, false, $dbins);	// 138: Es wurde(n) $var1 Kommunikationsdata/en erfolgreich hinzugefügt.
			if($dbupd)
				error_msg(139, false, $dbupd);	// 139: Es wurde(n) $var1 Kommunikationsdata/en erfolgreich geändert.
			if($dbdel)
				error_msg(140, false, $dbdel);	// 140: Es wurde(n) $var1 Kommunikationsdata/en erfolgreich gelöscht.
		}
	}
	else if(($GLOBALS['fxpglobals']['action'] == FXP_DELETE) && $_POST['Button_PZLoeschen'] && isset($_POST['Kommunikation_ID']))
		$dk=1;
}
if($err_kom)
	$err=true;

if(($da < 2) && ($dk == 1))
	error_msg(148);	// 148: Es wurden keine Kommunikationsdaten zum "Löschen" markiert.
if(($da == 1) && ($dk < 2))
	error_msg(147);	// 147: Es wurden keine Adressdaten zum "Löschen" markiert.

// Picture(s)
if(!$err && (($GLOBALS['fxpglobals']['action'] == FXP_CREATE) || (($GLOBALS['fxpglobals']['action'] == FXP_DELETE) && $_POST['Button_PGLoeschen'])))
{
	$ona=array(0=>convertFilename($_POST['Pname'].$_POST['Vorname']));
	if(strlen($old_pbname))
		$ona[1]=convertFilename($old_pbname);
	$ep=uploadPicture('PER', $_cid, $GLOBALS['fxpglobals']['action'], $_bpid, $ona, true);
fxDebug($ep, 'uploadPicture(\'PER\', $_cid='.$_cid.', $GLOBALS[\'fxpglobals\'][\'action\']='.$GLOBALS['fxpglobals']['action'].', $_bpid='.$_bpid.', $ona);', 2);
}

if(($da || $dk) && ($da < 2) && ($dk < 2))
	$err=true;
//$err=true;

// Send Emails if employee status has changed
if(!$err && sizeof($hdat) && (($hdat['personentyp'] == FXP_PT_EMPLOYEE) || ($hdat['personentyp'] == FXP_PT_CONTRACTOR)))
{
	$ndat=getPersonData($_bpid);
	if($hdat['inaktiv'] != $ndat['inaktiv'])
	{
		$mpa=getMailPersons(array(), 3065, $_bpid);	// Setting 3065: ...the entry/leave date or the status of an employee has been changed
		$sent=sendEmployeeDataMails($mpa, $_bpid, $ndat, $hdat);
		if(strlen($GLOBALS['_sent_success']) || strlen($GLOBALS['_sent_error']))
		{
			if(strlen($GLOBALS['_sent_error']))
			{
				$ec=substr_count($GLOBALS['_sent_error'], '<br />');
				if($ec == 1)
					error_msg(3069,false,':'.$GLOBALS['_sent_error']);			// 3069: The following person could not be informed by Email$var1
				else
					error_msg(3070,false,$ec,':'.$GLOBALS['_sent_error']);		// 3070: The following $var1 persons could not be informed by Email$var2
			}
			if(strlen($GLOBALS['_sent_success']))
			{
				$ec=substr_count($GLOBALS['_sent_success'], '<br />');
				if($ec == 1)
					error_msg(3071,false,':'.$GLOBALS['_sent_success']);		// 3071: The following person has been successfully informed by Email$var1
				else
					error_msg(3072,false,$ec,':'.$GLOBALS['_sent_success']);	// 3072: The following $var1 persons have been successfully informed by Email$var2
			}
		}
	}
}
//$err=true;

// No error -> Delete person rights
if(!$err)
{
	$persids=get_persons(true);
	// <B>NOTICE:</B> $var1 was successfully created, but you only have temporarily [<B>Change Rights</B>] for this person!
	if(($GLOBALS['fxpglobals']['action'] == FXP_CREATE) && $_bpid && fxIsArray($persids) && ($persids[$_bpid] == 2))
		error_msg(1812, false, $fmpname);
}

// No error -> Hide masks of deleted data
if(!$err && ($_POST['Kunde'] || (($GLOBALS['fxpglobals']['action'] == FXP_DELETE) && $_POST['Button_PGLoeschen'])))
{
	$maske387_nv=true;
	$maske383_nv=true;
	$maske484_nv=true;
	$maske384_nv=true;
	$maske386_nv=true;
	$maske388_nv=true;
}
else if(!$err && ($GLOBALS['fxpglobals']['action'] == FXP_DELETE) && $_pid && $_cid)
{
	// Hide address mask?
	$sql="SELECT adressen_id FROM adressen WHERE personen_id=".$_pid." AND mandanten_id=".$_cid;
	$aex=db_value($sql);
	if(!$aex)
		$maske384_nv=true;

	// Hide communication mask?
	$sql="SELECT kommunikation_id FROM kommunikation WHERE personen_id=".$_pid." AND mandanten_id=".$_cid;
	$kex=db_value($sql);
	if(!$kex)
		$maske386_nv=true;
}
?>