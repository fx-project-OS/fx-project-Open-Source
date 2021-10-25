<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : client_testdata.inc                                          //
// Version     : 21.2                                                         //
// Begin       : 2021-10-19                                                   //
// Last Change : 2021-10-19                                                   //
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
 * Check if client testdata can and should be installed - if not, skip
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$GLOBALS['__loaded_'.basename(__FILE__)]=true;


if(!fxIsArray($GLOBALS['_itla']))
	$GLOBALS['_itla']=fxGetLanguageTexts('init');

$__c1tdex=false;
$quit=false;
$now=fxNow();

// Check if it was already executed
$__sql="SELECT zeitstempel FROM artikel WHERE mandanten_id=0 AND artikel_id=1 AND personen_id=0 AND sachmittel_id IS NULL";
$__crt=db_value($__sql);
//fxDebug($__crt,$__sql, 0);
// ...YES, we can leave already
if($__crt)
	return;

// Check database if additional client data already exist
$__csa=array('benutzer'=>1, 'personen'=>2, 'projekte'=>0, 'rechte_trans'=>0, 'wertetab'=>0);
foreach($__csa as $__cn => $__ct)
{
	$__sql="SELECT COUNT(".$__cn."_id) AS cnt FROM ".$__cn." WHERE mandanten_id=1";
	$__crt=(int)db_value($__sql);
//fxDebug($__crt,$__sql, 0);
	if($__crt > $__ct)
	{
		$__c1tdex=true;
		break;
	}
}
//fxDebug($__c1tdex,'$__c1tdex', 0);
// ...YES, we can leave already
if($__c1tdex)
{
	insertClientTestdataMarker();
	return;
}

// Check if the testdata sql file exist
$__tdf=$GLOBALS['datpath'].$GLOBALS['fxpglobals']['dbparam']['versionscode_prg'].'_tdt.txt';
// ...NO, that's strange, but if we don't have it we cannot continue
if(!file_exists($__tdf))
{
	insertClientTestdataMarker();
	return;
}

// Click on "Continue without Test Data"?
if(fxIsArray($_POST) && isset($_POST['ctd_skip']))
{
	addFieldMessage('msg',$GLOBALS['_itla']['ctd_ins_skip'],100000);	// The test data was not created according to your request!<br /><br />You can now log in as "admin" in client 1 with the password you have assigned.
	insertClientTestdataMarker();
	return;
}

//fxDebug($_POST,'$_POST', 0);
if(!$GLOBALS['fxpglobals']['client'])
	$GLOBALS['fxpglobals']['client']=1;
if(!isset($_POST['ctd_pw']))
	$_POST['ctd_pw']='fx-project';
$_POST['ctd_pw']=trim($_POST['ctd_pw']);
$aktion=FXP_CREATE;
$GLOBALS['fxpglobals']['action']=$aktion;
$__cerr=true;

// Click on "Install Test Data"?
if(fxIsArray($_POST) && isset($_POST['ctd_inst']))
{
	$__cerr=false;
	$fa=fxLoad($__tdf, 1);
	if(fxIsArray($fa))
	{
//fxDebug($fa,$__tdf, 0);
		$pa=dbGetDataDiff($GLOBALS['fxpglobals']['dbparam']['versionscode_prg'],2,$fa);
		if($pa['status'] < 0)
			addFieldMessage('msg','<b>'.$__tdf.'</b><br />&rarr; '.$pa['message'],200000);
		else if(!fxIsArray($pa['sqlarray']))
			addFieldMessage('msg',$GLOBALS['_itla']['ctd_err_none'],200000);	// No valid test data inserts found!
		else
		{
			$swritelog='tdt_'.$GLOBALS['fxpglobals']['dbparam']['versionscode_prg'].'_'.$GLOBALS['datetime'];
			db_slog('#Installation Test Data: V'.$GLOBALS['fxpglobals']['dbparam']['version_prg'], __LINE__, __FILE__);

			$__etn='';
			$__ra_customer_adr=explode('|', $GLOBALS['_itla']['ctd_customer_adr']);	// Customer's Street|Customer's Place
			$__ra_persons=explode('|', $GLOBALS['_itla']['ctd_persons']);			// gm=General Manager|po=Project Office|pm=Project Manager|pl=Project Leader|pw=Project Worker|cs=Customer|cp=Customer's Contact Person
			$__ra_projects=explode('|', $GLOBALS['_itla']['ctd_projects']);			// Project|Mother-Project|Main-Project|Sub-Project|Task
//fxDebug($__ra_customer_adr,'$__ra_customer_adr', 0); fxDebug($__ra_persons,'$__ra_persons', 0); fxDebug($__ra_projects,'$__ra_projects', 0);

			db_rollback('start');
			foreach($pa['sqlarray'] as $__tdsql)
			{
				if(substr($__tdsql,0,1) == '*')
					db_slog('*TABLE: '.substr($__tdsql,1), __LINE__, __FILE__);
				else
				{
					$sa=db_sql2array($__tdsql);
					$__etn=$sa['table'];
					if(fxIsArray($sa['columns']))
					{
						if(isset($sa['columns']['zeitstempel']))
							$sa['columns']['zeitstempel']=substr($now,0,4).'0101000000';
						if(isset($sa['columns']['soll_beginn']) && ($sa['columns']['soll_beginn'] != 'NULL') && ($sa['table'] != 'madaten'))
							$sa['columns']['soll_beginn']=substr($now,0,4).substr($sa['columns']['soll_beginn'],4);
						if(isset($sa['columns']['soll_ende']) && ($sa['columns']['soll_ende'] != 'NULL'))
							$sa['columns']['soll_ende']=substr($now,0,4).substr($sa['columns']['soll_ende'],4);
						if(isset($sa['columns']['anlage_dtm']) && ($sa['columns']['anlage_dtm'] != 'NULL'))
							$sa['columns']['anlage_dtm']=substr($now,0,4).'0101000000';
						if(isset($sa['columns']['aenderungs_zeit']) && ($sa['columns']['aenderungs_zeit'] != 'NULL'))
							$sa['columns']['aenderungs_zeit']=substr($now,0,4).'0101000000';
						if(isset($sa['columns']['maeintrittsdatum']) && ($sa['columns']['maeintrittsdatum'] != 'NULL'))
							$sa['columns']['maeintrittsdatum']=substr($now,0,4).'0101000000';
						if(isset($sa['columns']['erstelldatum']) && ($sa['columns']['erstelldatum'] != 'NULL'))
							$sa['columns']['erstelldatum']=substr($now,0,4).'0101000000';
						if(isset($sa['columns']['parameter_txt']) && ($sa['columns']['parameter_txt'] != 'NULL'))
							$sa['columns']['parameter_txt']=substr($now,0,4).'-01-01-'.substr($sa['columns']['parameter_txt'],-6);
					}
					if($sa['table'] == 'adressen')
					{
						if(isset($__ra_customer_adr[0]))
							$sa['columns']['strasse']=$__ra_customer_adr[0];	// Customer's Street
						if(isset($__ra_customer_adr[1]))
							$sa['columns']['ort']=$__ra_customer_adr[1];		// Customer's Place
					}
					else if($sa['table'] == 'benutzer')
					{
						$uk=-1;
						$un=strtolower(trim($sa['columns']['name_benutzer']));
						if(($un == 'gm') || ($un == 'gf'))
							$uk=0;
						else if($un == 'po')
							$uk=1;
						else if($un == 'pm')
							$uk=2;
						else if($un == 'pl')
							$uk=3;
						else if(($un == 'pw') || ($un == 'mi'))
							$uk=4;
						if(($uk >= 0) && isset($__ra_persons[$uk]))
						{
							$__upa=explode('=', $__ra_persons[$uk]);
							$sa['columns']['name_benutzer']=$__upa[0];
						}
						$sa['columns']['passwort']=fxf_ecPasswd($_POST['ctd_pw'], '', $sa['columns']['mandanten_id'], $sa['columns']['benutzer_id']);
					}
					else if($sa['table'] == 'personen')
					{
						$pk=-1;
						$pn=strtr(strtolower(trim($sa['columns']['pname'])), array(' '=>'', '\''=>'', '-'=>''));
						if(($pn == 'generalmanager') || ($pn == 'geschäftsführer'))
							$pk=0;
						else if(($pn == 'projectoffice') || ($pn == 'projektoffice'))
							$pk=1;
						else if(($pn == 'projectmanager') || ($pn == 'projektmanager'))
							$pk=2;
						else if(($pn == 'projectleader') || ($pn == 'projektleiter'))
							$pk=3;
						else if(($pn == 'projectworker') || ($pn == 'projektmitarbeiter'))
							$pk=4;
						else if(($pn == 'customer') || ($pn == 'auftraggeber'))
							$pk=5;
						else if(($pn == 'customerscontactperson') || ($pn == 'auftraggeberansprechpartner'))
							$pk=6;
						if(($pk >= 0) && isset($__ra_persons[$pk]))
						{
							$__ppa=explode('=', $__ra_persons[$pk]);
							$sa['columns']['pname']=$__ppa[1];
						}
					}
					else if($sa['table'] == 'projekte')
					{
						$pk=-1;
						if($sa['columns']['projektart'] == FXP_PRJ_MAIN)
							$pk=0;
						else if($sa['columns']['projektart'] == FXP_PRJ_SUB)
							$pk=3;
						else if($sa['columns']['projektart'] == FXP_PRJ_TASK)
							$pk=4;
						if(($pk >= 0) && isset($__ra_projects[$pk]))
							$sa['columns']['name_projekt']=strtr(str_replace('-', '', strtolower(trim($sa['columns']['name_projekt']))), array('task'=>$__ra_projects[$pk], 'aufgabe'=>$__ra_projects[$pk], 'subproject'=>$__ra_projects[$pk], 'teilprojekt'=>$__ra_projects[$pk], 'main project'=>$__ra_projects[$pk], 'mainproject'=>$__ra_projects[$pk], 'hauptprojekt'=>$__ra_projects[$pk], 'mother project'=>$__ra_projects[$pk], 'motherproject'=>$__ra_projects[$pk], 'mutterprojekt'=>$__ra_projects[$pk], 'project'=>$__ra_projects[$pk], 'projekt'=>$__ra_projects[$pk]));
					}
					else if(($sa['table'] == 's_einstellungswerte') && ((int)$sa['columns']['einstellungs_id'] == 1))
						$sa['columns']['einstellungswert']=$GLOBALS['fxpglobals']['lang'];
					else if(($sa['table'] == 'zeitdaten') && ((int)$sa['columns']['zeitart'] == 105))
					{
						$sa['columns']['ist_beginn']=substr('0000'.((int)substr($now,0,4)-1),-4).'1200000000';
						$sa['columns']['ist_ende']=$sa['columns']['ist_beginn'];
					}
					$__sql=db_array2sql($sa);
//echo($__sql.'<hr />');
//db_slog('NEXT SQL = ['.$sql.']', __LINE__, __FILE__);
					$success=dbExecuteQuery($__sql);
					if(!$success)
					{
						$__cerr=true;
						break;
					}
				}
			}
//$__cerr=true;
			if($__cerr)
			{
				db_rollback('rollback');
				$emsg=str_replace('<!--#TDTN#-->', '<b>'.$__etn.'</b>', $GLOBALS['_itla']['ctd_err_insert']);	// An error occurred while inserting a test data set into the table &quot;<!--#TDTN-->&quot;! You can find more detailed information in the log file.
				addFieldMessage('msg',$emsg,300000);
				db_slog('*ERROR', __LINE__, __FILE__);
				db_slog('TABLE=['.$__etn.'], SQL=['.$__sql.']', __LINE__, __FILE__);
				$swritelog='';
			}
			else
			{
				db_rollback('ok');
				db_slog('*SUCCESS', __LINE__, __FILE__);
				$swritelog='';
				addFieldMessage('msg',$GLOBALS['_itla']['ctd_ins_ok'],100000);	// All test data has been successfully created.<br /><br />You can now log in to client 1 as "admin" or as one of the users "gm", "po", "pm", "pl" or "pw" with the password you have assigned.
			}
		}
	}
	else
		addFieldMessage('msg',str_replace('<!--#TDFN#-->', '<b>'.$__tdf.'</b>', $GLOBALS['_itla']['ctd_err_empty']),200000);	// The test data definition file &quot;<!--#TDFN#-->&uqot; for fx-project could not be loaded or is empty!
}
//$__cerr=true;

// Everything ok, then insert marker into db
if(!$__cerr)
{
	insertClientTestdataMarker();
	return;
}

// Create Client 1 Testdata
$twidth=970;
$tdupw=fieldText('ctd_pw', $_POST['ctd_pw'], floor($twidth/3), 32, 'left', true);

$content  = '<table width="'.$twidth.'" border="0" cellspacing="0" cellpadding="0">'.$nl;
$content .= '<tr>'.$nl;
$content .= '<td>'.$nl;
$content .= '<div class="maskc"><div class="maskt">&nbsp;'.$GLOBALS['_itla']['ctd_header'].'</div><br />'.$nl;	// Test Data
$content .= '<table width="100%" border="0" cellpadding="0" cellspacing="8">'.$nl;
$content .= '<tr>'.$nl;

$content .= '<td width="100%" height="100%" valign="top">'.$nl;
$content .= '<table width="100%" height="100%" cellpadding="8" cellspacing="0">'.$nl;
$content .= '<tr>'.$nl;
$content .= '<td valign="top" align="center"><span style="position:relative;top:-40px;font-size:150px;margin-right:16px;color:#006b9f;text-shadow:2px 2px 3px rgba(0,0,0,0.50);">+</span></td>'.$nl;
$content .= '<td valign="top">'.$nl;
$content .= '<div style="margin-right:16px;">'.$nl;

$content .= '<p align="justify">'.strtr($GLOBALS['_itla']['ctd_ins_note'], array('<!--#TDUPW#-->'=>$tdupw)).'</p><br /><br /><p align="justify" class="blue"><u>'.strtoupper($GLOBALS['_itla']['ms_important']).'!</u><br />'.$GLOBALS['_itla']['ctd_ins_del'].'</p>';

$content .= '</div><br />'.$nl;
$content .= '</td></tr></table>'.$nl;
$content .= '</td></tr></table>'.$nl;
$content .= '</div>'.$nl;
$content .= '</td></tr></table>'.$nl;

echo($content);

$button_inst=fieldSubmit('ctd_inst', $GLOBALS['_itla']['ctd_btn_inst'], 'sub', 0, false, false, true);	// Install Test Data
$button_skip=fieldSubmit('ctd_skip', $GLOBALS['_itla']['ctd_btn_skip'], 'sub', 0, false, false, true);	// Continue without Test Data

// Header
$GLOBALS['set_fxheader']=$GLOBALS['_itla']['ctd_header'];
$quit=true;
return;

function insertClientTestdataMarker()
{
	$sql  = "INSERT INTO artikel";
	$sql .= " (mandanten_id, artikel_id, personen_id, sachmittel_id,  historie, transid, aenderungs_id, zeitstempel)";
	$sql .= "VALUES";
	$sql .= " (0, 1, 0, NULL,  0, 0, 0, '".$GLOBALS['now']."')";
	@db_query($sql);
}
?>