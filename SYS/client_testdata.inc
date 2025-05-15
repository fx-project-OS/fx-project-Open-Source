<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : client_testdata.inc                                          //
// Version     : 24.1                                                         //
// Begin       : 2021-10-19                                                   //
// Last Change : 2024-04-29                                                   //
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
 * @version 24.1
 */

$GLOBALS['__loaded_'.basename(__FILE__)]=true;


if(!isset($GLOBALS['_itla']) || !fxIsArray($GLOBALS['_itla']))
	$GLOBALS['_itla']=fxGetLanguageTexts('init');

$__lang=fxGetLanguage();
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
	ctd_insertClientTestdataMarker();
	return;
}

// Check if the testdata sql file exist
$__tdf=$GLOBALS['datpath'].$GLOBALS['fxpglobals']['dbparam']['versionscode_prg'].'_tdt.txt';
// ...NO, that's strange, but if we don't have it we cannot continue
if(!file_exists($__tdf))
{
	ctd_insertClientTestdataMarker();
	return;
}

// Click on "Continue without Test Data"?
if(fxIsArray($_POST) && isset($_POST['ctd_skip']))
{
	addFieldMessage('msg',$GLOBALS['_itla']['ctd_ins_skip'],100000);	// The test data was not created according to your request!<br /><br />You can now log in as "admin" in client 1 with the password you have assigned.
	ctd_insertClientTestdataMarker();
	return;
}

//fxDebug($_POST,'$_POST', 0);
if(!$GLOBALS['fxpglobals']['client'])
	$GLOBALS['fxpglobals']['client']=1;
if(!isset($_POST['ctd_pw']))
	$_POST['ctd_pw']='fxproject';
$_POST['ctd_pw']=trim((string)$_POST['ctd_pw']);
$aktion=FXP_CREATE;
$GLOBALS['fxpglobals']['action']=$aktion;
$__cerr=true;

// Persons
$__ra_persons=explode('|', $GLOBALS['_itla']['ctd_persons']);	// gm=gm,General Manager,Moore,Gerald|po=po,Project Office,Owen,Pamela|pm=pm,Project Manager,Meyer,Paul|pl=pl,Project Leader,Lancaster,Phoebe|rp=rp,Resource Planner,Palmer,R.|em1=em1,Project Worker (Intern),Maddox,Eric|em2=em2,Project Worker (Intern),Mallard-Baker,Elizabeth|ca=ca,Customer,Customer AG LTD|ca-cp=ca-cp,Customer-Contact Person,Paulson,Caroline|ct=ct,Contact,Contact LLP|ct-cp=ct-cp,Contact-Contact Person,PillerCharles
//fxDebug($__ra_persons,'$__ra_persons', 0);
$__persons=array();
if(sizeof($__ra_persons))
{
	foreach($__ra_persons as $pv)
	{
		$pa=explode('=', $pv);
		if(sizeof($pa) > 1)
		{
			$pk=strtolower(trim($pa[0]));
			$__persons[$pk]=array('user'=>"", 'role'=>"", 'lname'=>"", 'fname'=>"");
			$paa=explode(',', $pa[1]);
			$__persons[$pk]['user']=strtolower(trim((string)$paa[0]));
			if(sizeof($paa) > 1)
				$__persons[$pk]['role']=trim((string)$paa[1]);
			if(sizeof($paa) > 2)
				$__persons[$pk]['lname']=trim((string)$paa[2]);
			if(sizeof($paa) > 3)
				$__persons[$pk]['fname']=trim((string)$paa[3]);
		}
	}
}
//fxDebug($__persons,'$__persons', 0);
$tducn=0;
$tdusr='<table>';
if(sizeof($__persons))
{
	foreach($__persons as $un => $ua)
	{
		if(strlen($ua['fname']))	// Only show entries with first name, i.e. don't display companies
		{
			$tducn++;
			$tdusr .= '<tr><td nowrap><b>'.strtolower($ua['user']).'</b></td><td nowrap>&nbsp;<span style="color:#888;">&rarr;</span>&nbsp;&nbsp;'.$ua['role'].'</td></tr>';	// e.g. gm (General Manager), po (Project Office), etc.
		}
	}
}
$tdusr .= '</table>';

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
		{
			addFieldMessage('msg','<b>'.$__tdf.'</b><br />&rarr; '.$pa['message'],200000);
			$__cerr=true;
		}
		else if(!fxIsArray($pa['sqlarray']))
		{
			addFieldMessage('msg',$GLOBALS['_itla']['ctd_err_none'],200000);	// No valid test data inserts found!
			$__cerr=true;
		}
		else
		{
			$swritelog='tdt_'.$GLOBALS['fxpglobals']['dbparam']['versionscode_prg'].'_'.$GLOBALS['datetime'];
			db_slog('#Installation Test Data: V'.$GLOBALS['fxpglobals']['dbparam']['version_prg'], __LINE__, __FILE__);

			$__etn='';
			$__ra_globals=explode('|', $GLOBALS['_itla']['ctd_globals']);				// Sample Company=Musterfirma|Customer AG=Auftraggeber AG|samplecompany=musterfirma|gm@=gf@|customer-ag=auftraggeber-ag|cp@=aa@|Completion=Fertigstellung|Development=Entwicklung|Manufacturing=Herstellung
			$__ra_adr_streets=explode('|', $GLOBALS['_itla']['ctd_adr_streets']);		// 123 Sample Street|Sample Avenue 123
			$__ra_adr_codes=explode('|', $GLOBALS['_itla']['ctd_adr_codes']);			// AA00 1BB
			$__ra_adr_places=explode('|', $GLOBALS['_itla']['ctd_adr_places']);			// SMPL-PLACE|SMPL-CITY
			$__ra_persons=explode('|', $GLOBALS['_itla']['ctd_persons']);				// GM=Moore,Gerald|CA=Customer AG LTD|CA-CP=Paulson,Caroline|PM=Meyer,Paul|PL=Lancaster,Phoebe|RP=Palmer,R.|PO=Owen,Pamela|EM1=Maddox,Eric|EM2=Mallard-Baker,Elizabeth
//fxDebug($__ra_globals,'$__ra_globals', 0); fxDebug($__ra_adr_streets,'$__ra_adr_streets', 0); fxDebug($__ra_adr_codes,'$__ra_adr_codes', 0); fxDebug($__ra_adr_places,'$__ra_adr_places', 0); fxDebug($__ra_persons,'$__ra_persons', 0);

			// Globals
			$__globals=array();
			if(sizeof($__ra_globals))
			{
				foreach($__ra_globals as $gv)
				{
					$ga=explode('=', $gv);
					if(sizeof($ga) > 1)
					{
						$gk=trim($ga[0]);
						$gr=trim($ga[1]);
						if(strlen($gk) && strlen($gr))
							$__globals[$gk]=$gr;
					}
				}
			}
//fxDebug($__globals,'$__globals', 0);

			// Company Email: @samplecompany.xyz
			$__email='@';
			if(sizeof($__ra_globals) && isset($__ra_company['Sample Company']))
				$__email .= strtolower(str_replace(' ', "", $__ra_company['Sample Company']));
			else if($__lang == 1)	// German
				$__email .= 'musterfirma';
			else					// English
				$__email .= 'samplecompany';
			$__email .= '.xyz';

			// Adjust sqls
			db_rollback('start');
			foreach($pa['sqlarray'] as $__tdsql)
			{
				if(substr((string)$__tdsql,0,1) == '*')
					db_slog('*TABLE: '.substr((string)$__tdsql,1), __LINE__, __FILE__);
				else
				{
					$sa=db_sql2array($__tdsql);
					$__etn=$sa['table'];

					// Adjust time values to current year for columns: zeitstempel, soll_beginn, soll_ende, anlage_dtm, aenderungs_zeit, maeintrittsdatum, erstelldatum, parameter_txt
					if(fxIsArray($sa['columns']))
					{
						if(isset($sa['columns']['zeitstempel']))
							$sa['columns']['zeitstempel']=substr((string)$now,0,4).'0101000000';
						if(isset($sa['columns']['soll_beginn']) && ($sa['columns']['soll_beginn'] != 'NULL') && ($sa['table'] != 'madaten'))
							$sa['columns']['soll_beginn']=substr((string)$now,0,4).substr((string)$sa['columns']['soll_beginn'],4);
						if(isset($sa['columns']['soll_ende']) && ($sa['columns']['soll_ende'] != 'NULL'))
							$sa['columns']['soll_ende']=substr((string)$now,0,4).substr((string)$sa['columns']['soll_ende'],4);
						if(isset($sa['columns']['anlage_dtm']) && ($sa['columns']['anlage_dtm'] != 'NULL'))
							$sa['columns']['anlage_dtm']=substr((string)$now,0,4).'0101000000';
						if(isset($sa['columns']['aenderungs_zeit']) && ($sa['columns']['aenderungs_zeit'] != 'NULL'))
							$sa['columns']['aenderungs_zeit']=substr((string)$now,0,4).'0101000000';
						if(isset($sa['columns']['maeintrittsdatum']) && ($sa['columns']['maeintrittsdatum'] != 'NULL'))
							$sa['columns']['maeintrittsdatum']=substr((string)$now,0,4).'0101000000';
						if(isset($sa['columns']['erstelldatum']) && ($sa['columns']['erstelldatum'] != 'NULL'))
							$sa['columns']['erstelldatum']=substr((string)$now,0,4).'0101000000';
						if(isset($sa['columns']['parameter_txt']) && ($sa['columns']['parameter_txt'] != 'NULL'))
							$sa['columns']['parameter_txt']=substr((string)$now,0,4).'-01-01-'.substr((string)$sa['columns']['parameter_txt'],-6);
					}

					// Adjust default language
					if(($sa['table'] == 's_einstellungswerte') && ((int)$sa['columns']['einstellungs_id'] == 1))
						$sa['columns']['einstellungswert']=$__lang;
					// Adjust carry values to current year
					else if(($sa['table'] == 'zeitdaten') && ((int)$sa['columns']['zeitart'] == 105))
					{
						$sa['columns']['ist_beginn']=substr('0000'.((int)substr($now,0,4)-1),-4).'1200000000';
						$sa['columns']['ist_ende']=$sa['columns']['ist_beginn'];
					}
					// User (=benutzer) values in selected language
					else if($sa['table'] == 'benutzer')
					{
						if($__lang != 7)	// 7=English is default, i.e. no replacements necessary
						{
							$un=strtolower(trim((string)$sa['columns']['name_benutzer']));
							$ua=ctd_getPerson($un);
							if(sizeof($ua))
							{
								// User name (=name_benutzer)
								$sa['columns']['name_benutzer']=strtolower($ua['user']);
							}
							else
								$sa['columns']['name_benutzer']=$un;
							// Email (=e_mail_adresse)
							$sa['columns']['e_mail_adresse']=$sa['columns']['name_benutzer'].$__email;
						}
						// Pasword (=passwort)
						$sa['columns']['passwort']=fxf_ecPasswd($_POST['ctd_pw'], '', $sa['columns']['mandanten_id'], $sa['columns']['benutzer_id']);
					}
					else if($__lang != 7)	// 7=English is default, i.e. no replacements necessary
					{
						// Address (=adressen) values in selected language
						if($sa['table'] == 'adressen')
						{
							// Street (=strasse) in selected language, e.g. 123 Sample Street
							if(is_array($__ra_adr_streets) && sizeof($__ra_adr_streets))
								$sa['columns']['strasse']=$__ra_adr_streets[random_int(0,sizeof($__ra_adr_streets)-1)];
							// Country (=landkz) in selected language, e.g. 847=D
							if($__lang == 1)	// German
								$sa['columns']['landkz']=847;
							// Code (=plz) in selected language, e.g. AA00 1BB
							if(is_array($__ra_adr_codes) && sizeof($__ra_adr_codes))
								$sa['columns']['plz']=$__ra_adr_codes[random_int(0,sizeof($__ra_adr_codes)-1)];
							// Place (=ort) in selected language, e.g. SMPL-PLACE
							if(is_array($__ra_adr_places) && sizeof($__ra_adr_places))
								$sa['columns']['ort']=$__ra_adr_places[random_int(0,sizeof($__ra_adr_places)-1)];
						}
						// Communication (=kommunikation) values in selected language
						else if(($sa['table'] == 'kommunikation') && sizeof($__globals))
						{
							// Communication Text (=kommunikation_txt)
							$sa['columns']['kommunikation_txt']=ctd_replaceGlobalTexts($sa['columns']['kommunikation_txt']);
						}
						// Milestones (=meilensteine) values in selected language
						else if(($sa['table'] == 'meilensteine') && sizeof($__globals))
						{
							// Milestone Name (=name_meilenstein)
							$sa['columns']['name_meilenstein']=ctd_replaceGlobalTexts($sa['columns']['name_meilenstein']);
						}
						// Persons (=personen) values in selected language
						else if(($sa['table'] == 'personen') && sizeof($__persons))
						{
							$un=strtolower(trim((string)$sa['columns']['personen_initialen']));
							$ua=ctd_getPerson($un);
							if(sizeof($ua))
							{
								// Name (=pname)
								$sa['columns']['pname']=$ua['lname'];
								// First Name (=vorname)
								$sa['columns']['vorname']=$ua['fname'];
								// Person's Initials (=personen_initialen)
								$sa['columns']['personen_initialen']=strtoupper($ua['user']);
							}
						}
						// Projects (=projekte) values in selected language
						else if(($sa['table'] == 'projekte') && sizeof($__globals))
						{
							$sa['columns']['name_projekt']=str_replace('.', '-', ctd_replaceGlobalTexts($sa['columns']['name_projekt']));
							$sa['columns']['beschreibung']=ctd_replaceGlobalTexts($sa['columns']['beschreibung']);
							$sa['columns']['port']=ctd_replaceGlobalTexts($sa['columns']['port']);
							$sp=strpos($sa['columns']['name_projekt'],' ');
							if(($sa['columns']['projektart'] == FXP_PRJ_TASK) && $sp && strlen($sa['columns']['beschreibung']))
								$sa['columns']['ref_nummer']=trim(substr($sa['columns']['name_projekt'],$sp+1)).'-'.strtoupper(substr($sa['columns']['beschreibung'], 0,1));	// Task 1-1 + Development -> 1-1-D
							else
								$sa['columns']['ref_nummer']='';
						}
						// Setting Values (=s_einstellungswerte) values in selected language
						else if(($sa['table'] == 's_einstellungswerte') && ($__lang == 1))
						{
							// Setting Value (=einstellungswert)
							if($sa['columns']['einstellungs_id'] == 1)														// 1 (=Language)			-> 1 (="Deutsch")
								$sa['columns']['einstellungswert']='1';
							else if(($sa['columns']['einstellungs_id'] == 2) || ($sa['columns']['einstellungs_id'] == 44))	// 2+44 (=DateFormat)		-> 940 (="dd.mm.yyyy")
								$sa['columns']['einstellungswert']='940';
							else if(($sa['columns']['einstellungs_id'] == 9) || ($sa['columns']['einstellungs_id'] == 46))	// 9+46 (=DecimalSeparater)	-> ","
								$sa['columns']['einstellungswert']=',';
							else if($sa['columns']['einstellungs_id'] == 27)												// 27 (=FirstDayOfWeek)		-> 1197 (="1 - Monday")
								$sa['columns']['einstellungswert']='1197';
							else if($sa['columns']['einstellungs_id'] == 96)												// 96 (=AppointmentProject)	-> "Termine"
								$sa['columns']['einstellungswert']='Termine';
						}
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
			}
			else
			{
				db_rollback('ok');
				db_slog('*SUCCESS', __LINE__, __FILE__);
				$swritelog='';
				addFieldMessage('msg',str_replace('<!--#TDUSR#-->',$tdusr,$GLOBALS['_itla']['ctd_ins_ok']),100000);	// All test data has been successfully created.<br /><br />You can now log in to client 1 as "<b>admin</b>" or as one of the following users with the password you have assigned:<br /><br /><!--#TDUSR#-->
			}
		}
	}
	else
	{
		addFieldMessage('msg',str_replace('<!--#TDFN#-->', '<b>'.$__tdf.'</b>', $GLOBALS['_itla']['ctd_err_empty']),200000);	// The test data definition file &quot;<!--#TDFN#-->&uqot; for fx-project could not be loaded or is empty!
		$__cerr=true;
	}
}
//$__cerr=true;

// Everything ok, then insert marker into db
if(!$__cerr)
{
	ctd_insertClientTestdataMarker();
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

$content .= '<p align="justify">'.strtr($GLOBALS['_itla']['ctd_ins_note'], array('<!--#TDUCN#-->'=>$tducn, '<!--#TDUSR#-->'=>$tdusr, '<!--#TDUPW#-->'=>$tdupw)).'</p><br /><br /><p align="justify" class="blue"><u>'.strtoupper((string)$GLOBALS['_itla']['ms_important']).'!</u><br />'.$GLOBALS['_itla']['ctd_ins_del'].'</p>';

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

function ctd_insertClientTestdataMarker()
{
	$sql  = "INSERT INTO artikel";
	$sql .= " (mandanten_id, artikel_id, personen_id, sachmittel_id,  historie, transid, aenderungs_id, zeitstempel)";
	$sql .= "VALUES";
	$sql .= " (0, 1, 0, NULL,  0, 0, 0, '".$GLOBALS['now']."')";
	@db_query($sql);
}

function ctd_replaceGlobalTexts($text)
{
	if(is_null($text))
		return '';

	if(!is_null($GLOBALS['__globals']) && is_array($GLOBALS['__globals']) && sizeof($GLOBALS['__globals']))
	{
		foreach($GLOBALS['__globals'] as $ck => $cr)
			$text=str_ireplace($ck, $cr, $text);
	}

	return $text;
}

function ctd_getPerson($user)
{
	$user=strtolower(trim((string)$user));

	if(strlen($user) && !is_null($GLOBALS['__persons']) && is_array($GLOBALS['__persons']) && sizeof($GLOBALS['__persons']) && isset($GLOBALS['__persons'][$user]))
		return $GLOBALS['__persons'][$user];
	return array();
}
?>