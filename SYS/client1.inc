<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : client1.inc                                                  //
// Version     : 21.2                                                         //
// Begin       : 2020-09-15                                                   //
// Last Change : 2021-03-19                                                   //
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
 * Check if client 1 exists - if not, create it
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$GLOBALS['__loaded_'.basename(__FILE__)]=true;


$__c1ex=true;
$quit=false;

// Check database if client 1 and admin exist
$__csa=array(
	'client1'=>array('table'=>'personen', 'sql'=>"SELECT personen_id, pname as name_mandant, name_zusatz as name_wert FROM personen WHERE mandanten_id=1 AND personentyp=".FXP_PT_CLIENT, 'result'=>false),
	'adminperson1'=>array('table'=>'personen', 'sql'=>"SELECT personen_id, anrede, perstitel, geschlecht, pname, vorname, name_zusatz, personen_initialen FROM personen WHERE mandanten_id=1 AND personentyp=".FXP_PT_CLIENT_CP, 'result'=>false),
	'adminuser1'=>array('table'=>'benutzer', 'sql'=>"SELECT benutzer_id, personen_id, name_benutzer, e_mail_adresse, passwort FROM benutzer WHERE mandanten_id=1 AND benutzer_id=1", 'result'=>false)
);
foreach($__csa as $__cn => $__ca)
{
	$__csa[$__cn]['result']=db_values($__ca['sql']);
	if(!fxIsArray($__csa[$__cn]['result']))
		$__c1ex=false;
}

// YES, we can leave already
if($__c1ex)
	return;

// Click on "Save"?
//fxDebug($__csa,'$__csa: $__c1ex='.$__c1ex, 0); fxDebug($_POST,'$_POST', 0);
$__cerr=false;
$__perr=false;
if(fxIsArray($_POST) && isset($_POST['Button_Multi']))
{
	// Validate fields
	$__va=array(
		'cname'=>'', 'cadd'=>'',
		'uname'=>'', 'email'=>'', 'pw'=>'', 'pwc'=>'',
		'asal'=>0, 'atit'=>0, 'asex'=>0, 'aname'=>'', 'afname'=>'', 'aadd'=>'', 'aidno'=>''
	);
	// ...User name is always lowercase
	if(strlen($_POST['Name_Benutzer']))
		$_POST['Name_Benutzer']=mb_strtolower(trim($_POST['Name_Benutzer']));
	// ...text field(s)
	$__fa=array('asal'=>'Anrede', 'atit'=>'PersTitel', 'asex'=>'Geschlecht');
	$__serr=validateSelectFields($__va, $__fa);
	if($__serr)
		$__cerr=true;
	// ...text field(s)
	$__fa=array('cname'=>'*Name_Mandant', 'cadd'=>'Name_Wert', 'uname'=>'*Name_Benutzer', 'email'=>'@E_Mail_Adresse', 'pw'=>'!Passwort_alias', 'pwc'=>'!Passwort_check', 'aname'=>'*Pname', 'afname'=>'Vorname', 'aadd'=>'Name_Zusatz', 'aidno'=>'Personen_Initialen');
	$__terr=validateTextFields($__va, $__fa);
	if($__terr)
		$__cerr=true;

	// Passwords must match
	if($__va['pw'] !== $__va['pwc'])
	{
		error_msg(860, array('Passwort_alias', 'Passwort_check')); // The "new" password does not correspond to the repeated entry.
		$__perr=true;
		$__cerr=true;
	}
//fxDebug($__va,'$__va', 0);

	// No error, insert values into database
	if(!$__cerr)
	{
		// ...Encode password
		$__dpw=fxf_ecPasswd($__va['pw'], '', 1, 1);

		// ...Start database transaction
		db_rollback();

		// ...Client 1
		$__cpid=0;
		$__mode='';
		$__where="";
		if(fxIsArray($__csa['client1']['result']))
		{
			$__cpid=(int)$__csa['client1']['result']['personen_id'];
			if(($__va['cname'] != $__csa['client1']['result']['name_mandant']) || ($__va['cadd'] != $__csa['client1']['result']['name_wert']))
			{
				$__mode='upd';
				$__where="personen_id=".$__cpid." AND mandanten_id=1";
				$__columns=array('pname'=>$__va['cname'], 'name_zusatz'=>$__va['cadd']);
			}
		}
		else
		{
			$__cpid=id_max('personen', 'personen_id');
			$__mode='ins';
			$__columns=array('mandanten_id'=>1, 'personen_id'=>$__cpid, 'personentyp'=>274, 'elter'=>0, 'pname'=>$__va['cname'], 'vorname'=>"", 'personen_initialen'=>"", 'name_zusatz'=>$__va['cadd'], 'anrede'=>0, 'perstitel'=>0, 'pposition'=>0, 'geschlecht'=>0, 'inaktiv'=>0);
		}
		if(strlen($__mode))
		{
			$__arr=array('mode'=>$__mode, 'table'=>'personen', 'where'=>$__where, 'columns'=>$__columns);
			$__sql=db_array2sql($__arr);
			if($__sql !== false)
			{
				$__dres=@db_query($__sql);
//fxDebug($__dres,'$__dres: $__sql='.$__sql, 0);
				if(!$__dres)
					$__cerr=true;
			}

			// ...Create default profiles
			if(!$__cerr && ($__mode == 'ins'))
			{
				$__cpa=fxf_getDefaultProfiles(1);
//fxDebug($__cpa,'$__cpa', 0);
				if(fxIsArray($__cpa))
				{
					foreach($__cpa as $__sql)
					{
						$__dres=@db_query($__sql);
//fxDebug($__dres,'$__dres: $__sql='.$__sql, 0);
						if(!$__dres)
						{
							$__cerr=true;
							break;
						}
					}
				}
			}
		}
		// ...Picture
		if(!$__cerr && $__cpid)
		{
			$__act=FXP_CREATE;
			$__cpa=array(0=>convertFilename($__va['cname']));
			if(fxIsArray($__csa['client1']['result']))
			{
				$__act=FXP_CHANGE;
				$__cpa[1]=convertFilename($__csa['client1']['result']['name_mandant']);
			}
			$__cperr=uploadPicture('PER', 1, $__act, $__cpid, $__cpa, 'nv_text1', true);
//fxDebug($__cperr, '$__cperr: uploadPicture(\'PER\',1, $__act='.$__act.', $__cpid='.$__cpid.', $__cpa, \'nv_text1\');', 0);
		}

		// ...Admin Person 1
		$__apid=0;
		$__mode='';
		$__where="";
		if(fxIsArray($__csa['adminperson1']['result']))
		{
			$__apid=(int)$__csa['adminperson1']['result']['personen_id'];
			if(($__va['asal'] != $__csa['adminperson1']['result']['anrede']) || ($__va['atit'] != $__csa['adminperson1']['result']['perstitel']) || ($__va['asex'] != $__csa['adminperson1']['result']['geschlecht']) || ($__va['aname'] != $__csa['adminperson1']['result']['pname']) || ($__va['afname'] != $__csa['adminperson1']['result']['vorname']) || ($__va['aadd'] != $__csa['adminperson1']['result']['name_zusatz']) || ($__va['aidno'] != $__csa['adminperson1']['result']['personen_initialen']))
			{
				$__mode='upd';
				$__where="personen_id=".$__apid." AND mandanten_id=1";
				$__columns=array('anrede'=>$__va['asal'], 'perstitel'=>$__va['atit'], 'geschlecht'=>$__va['asex'], 'pname'=>$__va['aname'], 'vorname'=>$__va['afname'], 'name_zusatz'=>$__va['cadd'], 'personen_initialen'=>$__va['aidno']);
			}
		}
		else
		{
			$__apid=id_max('personen', 'personen_id');
			$__mode='ins';
			$__columns=array('mandanten_id'=>1, 'personen_id'=>$__apid, 'personentyp'=>2090, 'elter'=>0, 'pname'=>$__va['aname'], 'vorname'=>$__va['afname'], 'personen_initialen'=>$__va['aidno'], 'name_zusatz'=>$__va['aadd'], 'anrede'=>$__va['asal'], 'perstitel'=>$__va['atit'], 'pposition'=>0, 'geschlecht'=>$__va['asex'], 'inaktiv'=>0);
		}
		if(strlen($__mode))
		{
			$__arr=array('mode'=>$__mode, 'table'=>'personen', 'where'=>$__where, 'columns'=>$__columns);
			$__sql=db_array2sql($__arr);
			if($__sql !== false)
			{
				$__dres=@db_query($__sql);
//fxDebug($__dres,'$__dres: $__sql='.$__sql, 0);
				if(!$__dres)
					$__cerr=true;
			}
		}
		// ...Picture
		if(!$__cerr && $__apid)
		{
			$__act=FXP_CREATE;
			$__cpa=array(0=>convertFilename($__va['aname'].$__va['afname']));
			if(fxIsArray($__csa['adminperson1']['result']))
			{
				$__act=FXP_CHANGE;
				$__cpa[1]=convertFilename($__csa['client1']['result']['pname'].$__csa['client1']['result']['vorname']);
			}
			$__cperr=uploadPicture('PER', 1, $__act, $__apid, $__cpa, 'nv_text2', true);
//fxDebug($__cperr, '$__cperr: uploadPicture(\'PER\',1, $__act='.$__act.', $__apid='.$__apid.', $__cpa, \'nv_text2\');', 0);
		}

		// ...Admin User 1
		$__mode='';
		$__where="";
		if(fxIsArray($__csa['adminuser1']['result']))
		{
			if(($__apid != $__csa['adminuser1']['result']['personen_id']) || ($__va['uname'] != $__csa['adminuser1']['result']['name_benutzer']) || ($__va['email'] != $__csa['adminuser1']['result']['e_mail_adresse']) || (strlen($__va['pw']) && ($__dpw != $__csa['adminuser1']['result']['passwort'])))
			{
				$__mode='upd';
				$__where="mandanten_id=1 AND benutzer_id=1";
				$__columns=array('personen_id'=>$__apid, 'name_benutzer'=>$__va['uname'], 'e_mail_adresse'=>$__va['email']);
				if(strlen($__va['pw']))
					$__columns['passwort']=$__dpw;
			}
		}
		else
		{
			$__mode='ins';
			$__columns=array('mandanten_id'=>1, 'benutzer_id'=>1, 'personen_id'=>$__apid, 'name_benutzer'=>$__va['uname'], 'e_mail_adresse'=>$__va['email'], 'passwort'=>$__dpw, 'b_admin'=>1, 'kosten'=>2160, 'login_zaehler'=>0, 'status_benutzer'=>0);
		}
		if(strlen($__mode))
		{
			$__arr=array('mode'=>$__mode, 'table'=>'benutzer', 'where'=>$__where, 'columns'=>$__columns);
			$__sql=db_array2sql($__arr);
			if($__sql !== false)
			{
				$__dres=@db_query($__sql);
//fxDebug($__dres,'$__dres: $__sql='.$__sql, 0);
				if(!$__dres)
					$__cerr=true;
			}
		}
//$__cerr=true;

		// Rollback all changes on error, else commit changes
		if($__cerr)
			db_rollback('error');
		else
			db_rollback('ok');
	}

	// Check database again if client 1 and admin exist now
	$__c1ex=true;
	foreach($__csa as $__cn => $__ca)
	{
		$__csa[$__cn]['result']=db_values($__ca['sql']);;
		if(!fxIsArray($__csa[$__cn]['result']))
			$__c1ex=false;
	}
//fxDebug($__csa,'$__csa: $__c1ex='.$__c1ex, 0);

	// YES, we can leave now
	if($__c1ex)
	{
		$__c1ex='client1';
		return;
	}
}

// Create Client 1 + Main Administrator
$aktion=FXP_CREATE;
$GLOBALS['fxpglobals']['action']=$aktion;
$maske18_sf=true;

// Get submitted, stored or default values for the following fields
$__fa=array(
	'Name_Mandant'=>'fx-project', 'Name_Wert'=>'',
	'Name_Benutzer'=>'admin', 'E_Mail_Adresse'=>'', 'Passwort_alias'=>'fx-project', 'Passwort_check'=>'fx-project',
	'Anrede'=>2803, 'PersTitel'=>0, 'Geschlecht'=>2802, 'Pname'=>'', 'Vorname'=>'', 'Name_Zusatz'=>'', 'Personen_Initialen'=>''
);
foreach($__fa as $__fn => $__fd)
{
	$__fl=strtolower($__fn);
	if(fxIsArray($_POST) && isset($_POST[$__fn]))
		$maskenfeldwerte[$__fl]=$_POST[$__fn];
	else if(fxIsArray($__csa['client1']['result']) && isset($__csa['client1']['result'][$__fl]))
		$maskenfeldwerte[$__fl]=$__csa['client1']['result'][$__fl];
	else if(fxIsArray($__csa['adminuser1']['result']) && isset($__csa['adminuser1']['result'][$__fl]))
		$maskenfeldwerte[$__fl]=$__csa['adminuser1']['result'][$__fl];
	else if(fxIsArray($__csa['adminperson1']['result']) && isset($__csa['adminperson1']['result'][$__fl]))
		$maskenfeldwerte[$__fl]=$__csa['adminperson1']['result'][$__fl];
	else
		$maskenfeldwerte[$__fl]=$__fd;
}
if(strlen($maskenfeldwerte['name_benutzer']))
	$maskenfeldwerte['name_benutzer']=mb_strtolower(trim($maskenfeldwerte['name_benutzer']));

// Client section
$lit_cl1=meldung(274).' 1';							// Client 1
$lit_adu=meldung(337).' ('.get_text(11,'20').')';	// Administrator (User)
$maskenfeldwerte['mandanten_id']='~<span class="h2 darkgrey tshadowlight">'.$lit_cl1.' + '.$lit_adu.'</span><br /><br />';
// ...Change literals for these fields
$Name_Mandant_lit=get_text(14,'20');	// Name
$Name_Wert_lit=get_text(16,'15');		// Name affix
// ...Password always empty because we cannot encrypt it, only compare it - except, it was just entered and produced an error
if(!$__perr)
{
	$maskenfeldwerte['passwort_alias']='';
	$maskenfeldwerte['passwort_check']='';
}
// ...Picture
if(fxIsArray($__csa['client1']['result']))
	$cp_pid=$__csa['client1']['result']['personen_id'];
else
	$cp_pid=0;
$cp_name=convertFilename(trim($maskenfeldwerte['name_mandant']));
$nv_text1_htext=get_text(1567,'10').' ['.$lit_cl1.']';	// Picture [Client 1]
if($__cerr)
	$maskenfeldwerte['nv_text1']=$_POST['nv_text1'];
else
	$maskenfeldwerte['nv_text1']=getPersonIcons($cp_pid,80,0,1,274,true);
// ...Uploader parameter
$nv_text1_ftype='pic';
$up='PER|1|2|'.$cp_pid.'|0|'.$cp_name;
echo(fieldHidden('nv_text1_uparam',$up));

// Main administrator section
$lit_adm=meldung(337);							// Administrator
$lit_adp=$lit_adm.' ('.get_text(21,'20').')';	// Administrator (Person)
$maskenfeldwerte['personen_id']='~<span class="h2 darkgrey tshadowlight">'.$lit_adp.'</span><br /><br />';
// ...Picture
if(fxIsArray($__csa['adminperson1']['result']))
	$ap_pid=$__csa['adminperson1']['result']['personen_id'];
else
	$ap_pid=0;
$ap_sex=(int)$maskenfeldwerte['geschlecht'];
$ap_name=convertFilename(trim($maskenfeldwerte['pname']).trim($maskenfeldwerte['vorname']));
$nv_text2_htext=get_text(1567,'10').' ['.$lit_adm.']';	// Picture [Administrator]
if($__cerr)
	$maskenfeldwerte['nv_text2']=$_POST['nv_text2'];
else
	$maskenfeldwerte['nv_text2']=getPersonIcons($ap_pid,80,$ap_sex,1,2090,true);
// ...Uploader parameter
$nv_text2_ftype='pic';
$up='PER|1|2|'.$ap_pid.'|'.$ap_sex.'|'.$ap_name;
echo(fieldHidden('nv_text2_uparam',$up));

// Display mask
echo(maske(18));

// Header
$GLOBALS['set_fxheader']=$lit_cl1.' + '.$lit_adm;
$quit=true;
return;
?>