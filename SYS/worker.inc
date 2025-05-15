<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : worker.inc                                                   //
// Version     : 24.1                                                         //
// Begin       : 2020-07-28                                                   //
// Last Change : 2024-10-22                                                   //
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
 * Framework that is called from Javascript via Ajax for a new program function, a new action mode or if data should be saved
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$GLOBALS['__loaded_'.basename(__FILE__)]=true;

// Debug this file? (Uncomment the next line)
//$GLOBALS['__debug'][__FILE__]=3;

// Search for and include "basics.inc" to set all definitions, variables and necessary dynamic paths
$__pnm='basics.inc';
if(!isset($GLOBALS['__loaded_'.$__pnm]))
{
	$__prg=$__pnm;
	$__pfc=0;
	while(($__pfc < 10) && !file_exists($__prg))
	{
		$__pfc++;
		$__prg='../'.$__prg;
	}
	if(file_exists($__prg))
	{
		require($__prg);
	}
	else
		die('<div style="padding:8px;"><h1 style="padding:8px;color:red;background:#fff;border:2px solid red;box-shadow:4px 4px 4px rgba(0,0,0, 0.5);">fx-project - ERROR: Necessary file &quot;'.$__pnm.'&quot; not found!</h1></div>');
}

// Uncomment next line to display the global runtime + memory
//$GLOBALS['__debug']['__DBG'] |= FXP_DEBUG_INF;


////////////////////////////////////////////////////////////////////////////////
// VARIABLES
////////////////////////////////////////////////////////////////////////////////
fxScrShtMod();
list($usec, $sec) = explode(' ', microtime());
$tdc0=$sec.substr((string)$usec, 1);

// Requested functionality
$fn='';
if(fxIsArray($_GET) && isset($_GET['fn']))
	$fn=strtolower((string)trim((string)$_GET['fn']));

// Current user language
if(fxIsArray($_GET) && isset($_GET['culang']))
	$culang=(int)$_GET['culang'];
else
	$culang=0;

if(strlen((string)$fn))
{
	if(!isset($_GET['keep_post']))
		$_POST=array();

	if($fn == 'matrix')
		$GLOBALS['nosession']=true;

	if(fxIsArray($GLOBALS['fxpglobals']) && fxIsArray($_GET))
	{
		if(isset($_GET['swidth']))
			$GLOBALS['fxpglobals']['swidth']=(int)$_GET['swidth'];
		if(isset($_GET['sheight']))
			$GLOBALS['fxpglobals']['sheight']=(int)$_GET['sheight'];
	}

	switch($fn)
	{
		case 'tr':		// Load transaction, click on submit button or action register
			ob_start();

			// Get internals
			$GLOBALS['__includer']['dms']=true;
			$GLOBALS['__includer']['person']=true;
			$GLOBALS['__includer']['resource']=true;
			// Get external: mailer (PHPMailer)
			$GLOBALS['__includer']['ext_mailer']=true;
			// Get additional person and project function collections
			$GLOBALS['__includer']['tools_pers']=true;
			$GLOBALS['__includer']['tools_prj']=true;
			require('includer.inc');

			if(fxIsArray($_GET) && isset($_GET['otr']))
				$otr=(int)trim((string)$_GET['otr']);
			else if(fxIsArray($_POST) && isset($_POST['otr']))
				$otr=(int)trim((string)$_POST['otr']);
			else
				$otr=0;

			$GLOBALS['_maskcounter']=1;
			$tpath='';
			$no_tpath=true;
			$ajaxret=array('tr'=>0, 'actions'=>"0000", 'status_type'=>'000', 'status_title'=>"<b>OK</b>", 'status_message'=>"");
			if(fxIsArray($_GET))
				extract($_GET);

			// Get button click element
			$uid='';
			if(fxIsArray($_GET) && isset($_GET['uid']) && !is_null($_GET['uid']))
				$uid=trim((string)$_GET['uid']);
			$aktion=0;
			if(strlen($uid) && (substr($uid,0,6) == 'actreg'))
				$aktion=(int)substr($uid,6,1);
			else if(fxIsArray($_GET) && isset($_GET['aktion']))
				$aktion=(int)trim((string)$_GET['aktion']);
			else if(fxIsArray($_POST) && isset($_POST['aktion']))
				$aktion=(int)trim((string)$_POST['aktion']);
			$frm='';
			if(fxIsArray($_GET) && isset($_GET['frm']) && !is_null($_GET['frm']))
				$frm=trim($_GET['frm']);
			if(!strlen($frm))
			{
				$fa=$aktion;
				if(!$fa)
					$fa=$GLOBALS['fxpglobals']['action'];
				$frm='_fxform_selmask_'.$fa;
			}
			$GLOBALS['popuptr']=0;
			if(fxIsArray($_GET) && isset($_GET['ptr']))
				$GLOBALS['popuptr']=(int)$_GET['ptr'];
			// Relogin?
			$assl=getSSL();
			if(($uid == 'logout') || (isset($GLOBALS['dbid']) && $GLOBALS['dbid'] && ($uid != 'fini_button_create') && ($assl !== $GLOBALS['fxpglobals']['ssl'])))
			{
				if($GLOBALS['fxpglobals']['client'])
					fxSession('g','s');

				$GLOBALS['fxpglobals']['user']=0;
				$GLOBALS['fxpglobals']['client']=0;
			}
			$lclick=0;
			if(fxIsArray($_GET) && isset($_GET['get']))
				$lclick=1;
			$bclick=0;
			$msl=(int)$_GET['msl'];
 			if(($uid == 'Button_Login') || ($uid == 'Button_2FA'))	// Always transmit menu structure and javascript variables for login
 			{
				fxf_dbFields(true);
 				$msl=0;
 			}
			$srtpst='';
			if(fxIsArray($_GET) && isset($_GET['srtpst']))
				$srtpst=$_GET['srtpst'];
			$GLOBALS['gproject']=0;
			if(fxIsArray($_GET))
			{
				if(isset($_GET['gproject']))
					$GLOBALS['gproject']=(int)$_GET['gproject'];
				else if(isset($_GET['gpn']))
				{
					$sql="SELECT projekt_id FROM projekte WHERE vorgangsnummer='".$_GET['gpn']."' AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$GLOBALS['gproject']=(int)db_value($sql);
				}
			}
			if(strlen((string)$uid))
			{
				if($uid == 'newtr')	// Click on menu
				{
					$tr=$_GET['ntr'];
					$nid='1';

					$_GET['tr']=$tr;
					$_GET['nid']=$nid;
				}
				else if(substr((string)$uid,0,6) == 'actreg')	// Click on action register
				{
					$tr=$otr;
					$aktion=(int)substr((string)$uid,6,1);
					$akfr='1';

					$_GET['tr']=$tr;
					$_GET['aktion']=$aktion;
					$_GET['akfr']=$akfr;
				}
				else if(($uid == 'get') || $lclick)	// Click on link
				{
					$lclick=1;
					extract($_GET);
				}
				else if(($uid != 'login') && ($uid != 'logout'))	// Click on submit button or image
				{
					$bclick=1;
					$uidvar=str2var($uid);
					$GLOBALS[$uidvar['var']]=$uidvar['value'];
					$tr=$otr;
					if(($uid != 'Button_Login') && ($uid != 'Button_2FA'))
						$db_update=true;

					$_POST[$uidvar['var']]=$uidvar['value'];
					$_POST['tr']=$tr;
					if(($uid != 'Button_Login') && ($uid != 'Button_2FA'))
						$_POST['db_update']=$db_update;

					if(fxIsArray($_GET) && isset($_GET['uidc']) && ($_GET['uidc'] == 'is'))	// submit image
					{
						$uidivar=str2var($uid,1);
						$GLOBALS[$uidivar['var'].'_x']=$uidivar['value'];
						$GLOBALS[$uidivar['var'].'_y']=$uidivar['value'];
						$_POST[$uidivar['var'].'_x']=$GLOBALS[$uidivar['var'].'_x'];
						$_POST[$uidivar['var'].'_y']=$GLOBALS[$uidivar['var'].'_y'];
					}
				}
			}

			if(!$GLOBALS['fxpglobals']['user'] || !$GLOBALS['fxpglobals']['client'] || ($uid == 'Button_Login') || ($uid == 'Button_2FA'))
			{
				$tr=0;
				$GLOBALS['fxpglobals']['tr']=$tr;
				$GLOBALS['fxpglobals']['person']=0;
				$GLOBALS['_form']=$frm;
				fxSession('t0','l');
				require('head.inc');
				$tr=$otr;
				$GLOBALS['fxpglobals']['tr']=$tr;
				if($err && !sizeof($ema) && !fxIsArray($GLOBALS['fxpvars']['msgarr']) && (($uid == 'Button_Login') || ($uid == 'Button_2FA')))
				{
					if($GLOBALS['fxpglobals']['lang'] == 1)
						$ema=array(2,'Loginhinweis','Login nicht erfolgreich');
					else
						$ema=array(2,'Login Notice','Login not successful');
				}
				if(sizeof($ema))
					$ajaxret['tr']=$otr;
				if(!$err && $GLOBALS['dbid'])
					$GLOBALS['fxpglobals']['ssl']=$assl;
			}

			if(sizeof($ema))
			{
				if(strlen((string)$ema[0]) == 3)
					$ajaxret['status_type']=$ema[0];
				else
				{
					$ema_st=(int)$ema[0];
					if($ema_st <= 1)
						$ajaxret['status_type']='001';	// green
					else if($ema_st == 2)
						$ajaxret['status_type']='010';	// yellow
					else
						$ajaxret['status_type']='100';	// red
				}
				$ajaxret['status_title']=$ema[1];
				$ajaxret['status_message']=$ema[2];
			}

			if(!$err)
			{
				if($GLOBALS['fxpglobals']['user'] > 0)
				{
					$__ultfa='';
					if(tr_program(284))
					{
						$uldfn=fxLoginDataFile();
						if(strlen($uldfn))
						{
							require($uldfn);
						}
//fxDebug($__ultfa, '$__ultfa: $us='.$us.', $uid='.$uid, 0);
					}

					// 2FA
					$__ultfa=(string)$__ultfa;
					if(strlen($__ultfa))
					{
						$GLOBALS['__fgul']=$GLOBALS['fxpglobals']['lang'];
						$GLOBALS['fxpglobals']['lang']=$GLOBALS['fxpglobals']['dbparam']['language'];
						require($GLOBALS['sppdir'][284].'/284_ini.inc');
						$GLOBALS['fxpglobals']['lang']=$GLOBALS['__fgul'];
					}

					// Login
					if(!strlen($__ultfa) && (($uid == 'Button_Login') || ($uid == 'Button_2FA')))
					{
						// Beim ersten Anmelden eines Benutzers, welcher kein normaler Mitarbeiter ist, d.h. Projektleiter, Projektmanager etc.
						$sql="SELECT login_zaehler,zeitstempel FROM benutzer WHERE benutzer_id=".$GLOBALS['fxpglobals']['user']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
						$ula=db_values($sql);
//fxDebug($ula,$sql, 0);
						if($GLOBALS['fxpglobals']['dhelp'] && $GLOBALS['fxpglobals']['persdat']['role'] && ($ula['login_zaehler'] < 3))
						{
							$_GET['tr']=158;
							$_GET['act']='help';
							$_GET['keep_get']=true;
							$GLOBALS['fxpglobals']['dhelp']=false;
						}
						// Start mit Kontrollzentrum?
						else
						{
							if(isset($GLOBALS['fxpglobals']['settings']['start_restore']) && (strtolower(substr((string)$GLOBALS['fxpglobals']['settings']['start_restore'],0,1)) == 'n'))
								$_GET['tr']=14;
						}
					}

					if(!strlen($__ultfa))
					{
						// Get program class
						getPrgClass();

						// Determine transaction
						$ntr=0;
						if(fxIsArray($_GET) && isset($_GET['tr']))
							$ntr=(int)trim((string)$_GET['tr']);
						else if(fxIsArray($_GET) && isset($_GET['ntr']))
							$ntr=(int)trim((string)$_GET['ntr']);
						else if(fxIsArray($_POST) && isset($_POST['tr']))
							$ntr=(int)trim((string)$_POST['tr']);
						else if(fxIsArray($_POST) && isset($_POST['ntr']))
							$ntr=(int)trim((string)$_POST['ntr']);
						if(!$ntr)
							$ntr=tGetTR();
						if(($ntr == 223) && !$GLOBALS['popuptr'])
							$ntr=0;
						if(!tr_rechte($ntr) || !$ntr)
						{
							if(($uid == 'Button_Login') || ($uid == 'Button_2FA'))
							{
								$lnk='';
								if(isset($GLOBALS['fxpglobals']['settings']['startseite']) && ($GLOBALS['fxpglobals']['settings']['startseite'] !== NULL))
									$lnk=$GLOBALS['fxpglobals']['settings']['startseite'];
								$trpos=strpos($lnk,'?tr=');
								if($trpos)
								{
									$ntr=(int)substr((string)$lnk,$trpos+4);
								}
								else
								{
									$trpos=strpos($lnk,'&tr=');
									if($trpos)
									{
										$ntr=(int)substr((string)$lnk,$trpos+4);
									}
								}
							}
							else
							{
								$_GET['act']='itr'.$ntr;
								$_GET['keep_get']=true;
							}
							if(!tr_rechte($ntr))	// No rights (anmyore) to default start transaction -> display version
								$ntr=158;
						}

						$tr=$ntr;
						tSetTR($tr,$otr);
						$ajaxret['tr']=$tr;

						// Uncheck projects?
						if($otr && ($otr != $ntr))
						{
							$otrpva=prjValidPF(true);
							if(isset($otrpva[$otr]))
								@prjCheckIn();
						}

						// Call transaction
						if($tr != 0)
						{
							trStack();

							$GLOBALS['_maskcounter']=1;

							// Only submitted form?
							$sameform=false;
							if(strpos($frm,'lstmask') !== false)
								$sameform=true;
							else
							{
								$tform=db_value("SELECT form FROM trans_maske WHERE transakt_id=".$tr." AND form IS NOT NULL");
								if(strlen((string)$tform))
									$sameform=true;
							}

							// Load transaction data -> POST
							$GLOBALS['fxptdata']=array();
							$GLOBALS['_form']=$frm;
							fxSession('t','l','','',true,$sameform);
							if($lclick)
								$_POST=array();
							if($bclick && !isset($_GET['keep_get']))
								$_GET=array();
							if(strlen((string)$srtpst))
							{
								$_POST['sort_post']=$srtpst;
								$GLOBALS['sort_post']=$srtpst;
							}
							if(($uid == 'Button_Login') || ($uid == 'Button_2FA'))
							{
								$frm='';
								$GLOBALS['_form']=$frm;
							}

							// Set/Get action: 16=Delete, 8=Change, 4=Create, 2=Display + 1=None
							$tra=tr_rechte($GLOBALS['fxpglobals']['tr'], $GLOBALS['fxpglobals']['user'], false);
							$trr=tr_rechte($GLOBALS['fxpglobals']['tr'], $GLOBALS['fxpglobals']['user'], true);
							$trr=max(0,$trr);
							// Clients>1 are not allowed to create new clients
							if(($GLOBALS['fxpglobals']['tr'] == 166) && ($GLOBALS['fxpglobals']['client'] > 1))
								$trr &= 27;
							if(!$aktion)
								$aktion=$GLOBALS['fxptdata']['action'];
							if(!$aktion)
								$aktion=$tra['da'];
							if(!$aktion)
								$aktion=1;

							// Check, which actions are allowed
							if($trr > 0)
							{
								$tr_r_array=array(1=>1,1,1,1);
								$act_action=false;
								$new_action=0;
								for($act_code=1; $act_code<5; $act_code++)
								{
									// Check, if action is allowed
									$act_stat=test_bit($trr, $act_code);
									if(!$act_stat && !$not_disabled)	// No, set to 0 in array
										$tr_r_array[$act_code]=0;
									else if(!$new_action)				// else remember first action found
										$new_action=$act_code;

									// Check, if actual action is allowed
									if(($tr_r_array[$act_code] == 1) && ($aktion == $act_code))
									{
										$tr_r_array[$aktion]=2; // Actual action
										$act_action=true;
									}
								}

								// If actual action is not allowed switch to first found action
								if(!$act_action && $new_action)
								{
									$aktion=$new_action;
									$tr_r_array[$aktion]=2; // Actual action
								}

								// Special: Test action for special profiles
								fxpTestLogAction();

								$ajaxret['actions']='';
								for($act_code=1; $act_code<5; $act_code++)
									$ajaxret['actions'] .= $tr_r_array[$act_code];

								$GLOBALS['fxpglobals']['action']=$GLOBALS['fxptdata']['action'];
							}
							else
								$aktion=-1;

							$GLOBALS['fxptdata']['action']=$aktion;
							$GLOBALS['fxpglobals']['action']=$aktion;

							// In display mode no "db_update"
							if(isset($db_update) && (($aktion == FXP_DISPLAY) || isset($_POST['back']) || fxIsArray($_GET)))
							{
								unset($db_update);
								unset($_POST['db_update']);
							}

							// Main routine is done here...
							if(!isset($retransmit))
								$retransmit=false;
							require('detail.inc');

							// Check, if menu structure or settings must be retransmitted
							$ca=fxf_getClientFilenames(-1);
//echo('datetime='.$GLOBALS['datetime'].', menudate='.$GLOBALS['fxpglobals']['menudate'].'<br />');
							if(!$msl || !isset($GLOBALS['fxpglobals']['menudate']) || !file_exists($ca['g']))
								$retransmit=true;
							if(!$retransmit)
							{
								$rtca=array('s_einstellungswerte'=>"",'benutzer'=>"benutzer_id=".$GLOBALS['fxpglobals']['user'],'benutzergruppen'=>"",'benutzergruppenzo'=>"",'rechte_trans'=>"");
								foreach($rtca as $rtct => $rtcc)
								{
									$csql="SELECT MAX(zeitstempel) AS mdt FROM ".$rtct." WHERE mandanten_id=".$GLOBALS['fxpglobals']['client'];
									if(strlen((string)$rtcc))
										$csql .= " AND ".$rtcc;
									$rtcdt=db_value($csql);
//fxDebug($rtcdt,$csql);
									if(($rtcdt !== NULL) && strlen((string)$rtcdt) && ($rtcdt > $GLOBALS['fxpglobals']['menudate']))
									{
										$retransmit=true;
										break;
									}
								}
							}

							// Transmit menu structure, javascript variables, languages and person infos?
							if($retransmit)
							{
								// Menu structure, entries and colors
								require('config_menu.inc');

								// Language(s)
								$sla=fxGetAllLanguages(false,true);
//fxDebug($sla, '$sla', 0);
								if(!isset($sla[$GLOBALS['fxpglobals']['lang']]))
									$GLOBALS['fxpglobals']['lang']=fxGetLanguage(0,false);
//echo('<b>Retransmit</b>... lang='.$GLOBALS['fxpglobals']['lang'].'<br />');

								// Javascript variables
								if(!fxIsArray($GLOBALS['fxpglobals']['settings']) || !isset($GLOBALS['fxpglobals']['settings']['formatdatum']))
									$GLOBALS['fxpglobals']['settings']=benutzereinstellungen($GLOBALS['fxpglobals']['user']);
								if(fxIsArray($GLOBALS['fxpglobals']['persdat']))
									$jspn=$GLOBALS['fxpglobals']['persdat']['name1'];
								else
									$jspn='{'.$GLOBALS['fxpglobals']['person'].'}';

								echo('<div id="oset" style="display:none;">');

								echo('client=I'.$GLOBALS['fxpglobals']['client']);
								echo('|user=I'.$GLOBALS['fxpglobals']['user']);
								echo('|person=I'.$GLOBALS['fxpglobals']['person']);
								echo('|ccenter=I'.tr_rechte(14));
								echo('|pname=S'.fxReplaceSpecialCharacters($jspn,false));
								echo('|dateformat=S'.fxReplaceSpecialCharacters($GLOBALS['fxpglobals']['settings']['formatdatum'],false));
								echo('|timeformat=S'.fxReplaceSpecialCharacters($GLOBALS['fxpglobals']['settings']['formatzeit'],false));
								echo('|decimalpoint=S'.fxReplaceSpecialCharacters($GLOBALS['fxpglobals']['settings']['deztrennzeichen'],false));
								echo('|decimallength=I'.$GLOBALS['fxpglobals']['settings']['nkstellen']);
								echo('|currency=S'.fxReplaceSpecialCharacters($GLOBALS['fxpglobals']['settings']['waehrung'],false));

								echo('|punit=I'.$GLOBALS['fxpglobals']['punit']);
								echo('|cunit=I'.$GLOBALS['fxpglobals']['settings']['munit']);
								echo('|unitselect=AS0,S856,S484,S485,S923,S486,S487');
								echo('|unitcalc=AI3600,I60,I3600,I'.($GLOBALS['fxpglobals']['settings']['mtag']*3600).',I'.($GLOBALS['fxpglobals']['settings']['mwoche']*$GLOBALS['fxpglobals']['settings']['mtag']*3600).',I'.($GLOBALS['fxpglobals']['settings']['mmonat']*$GLOBALS['fxpglobals']['settings']['mtag']*3600).',I'.($GLOBALS['fxpglobals']['settings']['mjahr']*$GLOBALS['fxpglobals']['settings']['mtag']*3600));
								echo('|unitlit=AS'.meldung(484).',S'.meldung(856).',S'.meldung(484).',S'.meldung(485).',S'.meldung(923).',S'.meldung(486).',S'.meldung(487));

								echo('|timedec=S'.strtolower(substr((string)$GLOBALS['fxpglobals']['settings']['zeitspanne_dez'],0,1)));	// 'j'a oder 'n'ein

								echo('|lit_hours=S'.fxReplaceSpecialCharacters(meldung(1174,false),false));
								echo('|lit_daytype=S'.fxReplaceSpecialCharacters(get_text(439,'15'),false));
								echo('|lit_endtime=S'.fxReplaceSpecialCharacters(get_text(76,'10'),false));
								echo('|half_vacations=S'.strtolower(substr((string)$GLOBALS['fxpglobals']['settings']['halbe_urlaube'],0,1)));

								echo('|cost_km=AS'.$GLOBALS['fxpglobals']['settings']['rk_km_2213'].',S'.$GLOBALS['fxpglobals']['settings']['rk_km_3403'].',S'.$GLOBALS['fxpglobals']['settings']['rk_km_1951'].',S'.$GLOBALS['fxpglobals']['settings']['rk_km_1952'].',S'.$GLOBALS['fxpglobals']['settings']['rk_km_1953']);
								echo('|cost_pass=AF'.get_double($GLOBALS['fxpglobals']['settings']['rk_km_2214']).',F'.get_double($GLOBALS['fxpglobals']['settings']['rk_km_1954']));

								echo('|cost_btype=S'.strtoupper(substr((string)$GLOBALS['fxpglobals']['settings']['budgetart'],0,1)));	// 'N'one, 'I'ntern, 'E'xtern or 'B'oth
								echo('|cost_see=I'.darf_kosten_sehen());	// 0=None, 1=Intern, 2=Extern or 3=Both

								echo('|budappr_auto=S'.strtolower(substr((string)$GLOBALS['fxpglobals']['settings']['budgetgen'],0,1)));	// 'j'a oder 'n'ein
								echo('|budappr_active=S'.strtolower(substr((string)$GLOBALS['fxpglobals']['settings']['budgetgenaktiv'],0,1)));	// 'j'a oder 'n'ein

								echo('|adjust_meffort=S'.strtolower(substr((string)$GLOBALS['fxpglobals']['settings']['max_aufw_anpass'],0,1)));	// 'j'a oder 'n'ein

								echo('|rplan_hrs=S'.strtolower(substr((string)$GLOBALS['fxpglobals']['settings']['std_statt_prz'],0,1)));	// 'j'a oder 'n'ein

								echo('|sort09=S'.strtolower(substr((string)$GLOBALS['fxpglobals']['settings']['vmn_projektnr'],0,1)));	// 'j'a oder 'n'ein

								$scla=getSettingMaxCL($GLOBALS['fxpglobals']['settings']['sel_max_cl']);
								echo('|sel_max_c=I'.$scla['chars']);
								echo('|sel_max_l=I'.$scla['lines']);

								echo('|act_fs_dep=S'.strtolower(substr((string)$GLOBALS['fxpglobals']['settings']['aa_aktivieren'],0,1)));

								echo('|setfilter=I'.$GLOBALS['fxpglobals']['setfilter']);
								echo('|sethelp=I'.$GLOBALS['fxpglobals']['sethelp']);
								echo('|settrmark=I'.$GLOBALS['fxpglobals']['settrmark']);

								echo('|setppsppopalways=I'.$GLOBALS['fxpglobals']['setppsppopalways']);

								echo('|setalignworkday=I'.$GLOBALS['fxpglobals']['setalignworkday']);
								echo('|setshowframes=I'.$GLOBALS['fxpglobals']['setshowframes']);
								echo('|setshowlines=I'.$GLOBALS['fxpglobals']['setshowlines']);
								echo('|setshowhr=I'.$GLOBALS['fxpglobals']['setshowhr']);
								echo('|setcontnumbers=I'.$GLOBALS['fxpglobals']['setcontnumbers']);

								echo('|setwfcolor=I'.$GLOBALS['fxpglobals']['setwfcolor']);
								echo('|setwfairow=I'.$GLOBALS['fxpglobals']['setwfairow']);

								echo('|msg_prefix=S'.fxReplaceSpecialCharacters(meldung(2303, false),false));

								echo('</div>'.$nl);

								// Filter
								getFilters(true);

								// Person info
								// ...Pic
								if(!$GLOBALS['fxpglobals']['person'])
								{
									if($GLOBALS['fxpglobals']['lang'] == 1)	// Deutsch
										$taskpinfo_name='Systemadministrator';
									else
										$taskpinfo_name='System Administrator';
									$taskpinfo_fname=$taskpinfo_name;
								}
								else
								{
									$taskpinfo_name=trim((string)$GLOBALS['fxpglobals']['persdat']['pname']);
									$taskpinfo_fname=$taskpinfo_name;
									if(strlen((string)trim((string)$GLOBALS['fxpglobals']['persdat']['vorname'])))
									{
										$taskpinfo_name = trim((string)$GLOBALS['fxpglobals']['persdat']['vorname']).' '.$taskpinfo_name;
										$taskpinfo_fname .= ', '.trim((string)$GLOBALS['fxpglobals']['persdat']['vorname']);
									}
								}
								$pmxh=128;
								$pers_icons=getPersonIcons($GLOBALS['fxpglobals']['person'], $pmxh, $GLOBALS['fxpglobals']['persdat']['geschlecht'], $GLOBALS['fxpglobals']['client'], $GLOBALS['fxpglobals']['persdat']['personentyp']);
								$dwh='dsp';
								if(!$pers_icons['def'] && strlen((string)$pers_icons['dsp']))
								{
									// Größte Größe ermitteln
									$wha=explode(' ',$pers_icons['size_dsp']);
									$dw=(int)substr((string)$wha[1],6);
									$dh=(int)substr((string)$wha[2],7);
									if($dh < 50)
									{
										if(isset($pers_icons['size_128']) && (strlen((string)$pers_icons['size_128']) > 0))
											$dwh='128';
										else if(isset($pers_icons['size_100']) && (strlen((string)$pers_icons['size_100']) > 0))
											$dwh='100';
									}
								}
								$taskpinfo_fpic  = '<div style="position:relative;margin-right:24px;"><img src="'.$pers_icons['dsp'].$pers_icons['mtime'].'"'.$pers_icons['size_'.$dwh].' align="right" style="max-height:'.$pmxh.'px;width:auto;margin-right:4px;border:1px solid white;border-right-color:black;border-bottom-color:black;border-radius:8px;box-shadow:3px 3px 6px rgba(0,0,0, 0.5);">';
								$taskpinfo_fpic .= '<img src="'.$GLOBALS['gfxpath'].'atc_c_80x80.png" style="position:absolute;right:-8px;top:-46px;"></div>';

								if(($GLOBALS['fxpglobals']['user'] == 1) && strpos($taskpinfo_fpic, '/GFX/'))
									$taskpinfo_fname='Administrator';
								// ...Type
								if($GLOBALS['fxpglobals']['persdat']['personentyp'] == 99)
									$taskpinfo_type='fx-Administration';
								else
									$taskpinfo_type=fxReplaceSpecialCharacters(meldung($GLOBALS['fxpglobals']['persdat']['personentyp'],true),false);
								// ...Role(s)
								$tpir=$GLOBALS['fxpglobals']['persdat']['role'];
								$taskpinfo_role='';
								if(!$GLOBALS['fxpglobals']['user'])
								{
									if($GLOBALS['fxpglobals']['lang'] == 1)	// Deutsch
										$taskpinfo_role='Systemadministrator';
									else
										$taskpinfo_role='System Administrator';
									$tpir=666;
								}
								else if($GLOBALS['fxpglobals']['user'] == 1)
								{
									$taskpinfo_role='Administrator';
									$tpir=666;
								}
								else if($GLOBALS['fxpglobals']['persdat']['role'])
									$taskpinfo_role=fxReplaceSpecialCharacters(db_value("SELECT mafaehigkeit_name FROM ma_faehigkeit WHERE mafaehigkeit_id=".$GLOBALS['fxpglobals']['persdat']['role']." AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id=0"),false);
								$taskpinfo_addrole='';
								if(fxIsArray($GLOBALS['fxpglobals']['persdat']['roles']))
								{
									foreach($GLOBALS['fxpglobals']['persdat']['roles'] as $arid => $dummy)
									{
										if($arid && ($arid != $tpir))
										{
											$artx=fxReplaceSpecialCharacters(db_value("SELECT mafaehigkeit_name FROM ma_faehigkeit WHERE mafaehigkeit_id=".$arid." AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")"),false);
											if(strlen((string)$artx))
											{
												if(!strlen((string)$taskpinfo_role))
													$taskpinfo_role = $artx;
												else
												{
													if(!strlen((string)$taskpinfo_addrole))
														$taskpinfo_addrole="<br /><i>+</i> ";
													else
														$taskpinfo_addrole .= "<i>,</i> ";
													$taskpinfo_addrole .= "<i>".$artx."</i>";
												}
											}
										}
									}
								}
								if(!strlen((string)$taskpinfo_role))
									$taskpinfo_role=fxReplaceSpecialCharacters(meldung(1700,true),false);	// Standard
								// ...Admin
								$aimg='0';
								$atxt='';
								if(!$GLOBALS['fxpglobals']['person'])	// System Administrator
								{
									$aimg='1';
									if($GLOBALS['fxpglobals']['lang'] == 1)	// Deutsch
										$atxt='Systemadministrator';
									else
										$atxt='System Administrator';
								}
								else if($GLOBALS['fxpglobals']['user'] == 1)	// Main Administrator
								{
									$aimg='1';
									if($GLOBALS['fxpglobals']['lang'] == 1)	// Deutsch
										$atxt='Hauptadministrator';
									else
										$atxt='Main Administrator';
								}
								else if($GLOBALS['fxpglobals']['useradmin'])
									$aimg='1';
								if(strlen((string)$atxt))
									$atxt='&nbsp;<i class="grey">('.$atxt.')</i>';
								$taskpinfo_admin='<img src="'.$GLOBALS['gfxpath'].$aimg.'.png">'.$atxt;
								// ...Email+Login
								$taskpinfo_email='';
								$taskpinfo_liarr=db_values("SELECT login_zaehler, zeitstempel, e_mail_adresse FROM benutzer WHERE benutzer_id=".$GLOBALS['fxpglobals']['user']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
								if(fxIsArray($taskpinfo_liarr))
								{
									if(strlen((string)$taskpinfo_liarr['e_mail_adresse']))
										$taskpinfo_email=fxReplaceSpecialCharacters($taskpinfo_liarr['e_mail_adresse'],false);
									$taskpinfo_login="#".$taskpinfo_liarr['login_zaehler'];
								}
								else
									$taskpinfo_login="#1";
								$taskpinfo_login .= " <i class=grey>(".substr((string)$GLOBALS['fxpglobals']['sesstoid'],1)."-".substr((string)$GLOBALS['lts'],0,8)."-".substr((string)$GLOBALS['lts'],8).")</i>";

								if($GLOBALS['fxpglobals']['lang'] == 1)	// Deutsch
									$pila=array('Mandant','Typ','Rolle','Admin','Email','Login');
								else
									$pila=array('Client','Type','Role','Admin','Email','Login');

								echo('<div id="taskpinfo" style="display:none;">');
								echo($GLOBALS['fxpglobals']['client'].": ".fxReplaceSpecialCharacters($taskpinfo_name,false));
								echo("|<table>");
								echo("<tr>");
								echo("<td valign=top>".$taskpinfo_fpic."</td>");
								echo("<td valign=top nowrap>");
								echo("<table>");
								echo("<tr><td colspan=2><b>".fxReplaceSpecialCharacters($taskpinfo_name,false)."</b><br /><font class=grey>[ID ".$GLOBALS['fxpglobals']['person']."]</font><br /><br /></td></tr>");
								echo("<tr><td class=lit valign=top>".$pila[0].":&nbsp;</td><td nowrap valign=top>".$GLOBALS['fxpglobals']['client']."</td></tr>");		// Client
								echo("<tr><td class=lit valign=top>".$pila[1].":&nbsp;</td><td nowrap valign=top>".$taskpinfo_type."</td></tr>");						// Type
								echo("<tr><td class=lit valign=top>".$pila[2].":&nbsp;</td><td nowrap valign=top>".$taskpinfo_role.$taskpinfo_addrole."</td></tr>");	// Role
								echo("<tr><td class=lit valign=top>".$pila[3].":&nbsp;</td><td nowrap valign=top>".$taskpinfo_admin."</td></tr>");						// Admin
								if(strlen((string)$taskpinfo_email))
									echo("<tr><td class=lit valign=top><br />".$pila[4].":&nbsp;</td><td nowrap valign=top><br />".$taskpinfo_email."</td></tr>");			// Email
								if(strlen((string)$taskpinfo_login))
									echo("<tr><td class=lit valign=top><br />".$pila[5].":&nbsp;</td><td nowrap valign=top><br />".$taskpinfo_login."</i></td></tr>");		// Login
								echo("</table>");
								echo("</td></tr></table>");
								echo('</div>'.$nl);

								// Design
								tGetDesign();
								echo('<div id="ps_design" style="display:none;">'.$GLOBALS['ps_design'].'</div>');
								$fxdst=fxpGetDesigns();
								echo('<div id="tb_design" class="" style="display:none;">'.$fxdst.'</div>');
							}
						}
					}
				}
			}

			// Status message(s)
			if(!sizeof($ema))
			{
				$ajaxret['status_title']='<b class=green>OK</b>';
				$mtask='';
				if(fxIsArray($GLOBALS['fxpvars']['msgarr']))
				{
					$msgcnt=1;
					$sttlr='0';
					$sttly='0';
					$sttlg='0';
					$ajaxret['status_message']='<table width="100%" border="0" cellpadding="0" cellspacing="1">';
					krsort($GLOBALS['fxpvars']['msgarr'], SORT_NUMERIC);
					foreach($GLOBALS['fxpvars']['msgarr'] as $errdeg => $msg)
					{
						krsort($msg, SORT_NUMERIC);
						foreach($msg as $msgcont)
						{
							// Adjust message array for transactions without array sort
							$msg_id_txt='id_tr';
							if((isset($msgcont['col']) && ($msgcont['col'] !== NULL) && strlen((string)$msgcont['col'])) || ($msgcont['nr'] != $tr))
							{
								if(!$errdeg)
								{
									if(!isset($msgcont['col']) || ($msgcont['col'] === NULL) || !strlen((string)$msgcont['col']))
										$msgcont['col'] = 200000;
									$errdeg = (int)$msgcont['col'];
								}
								$msg_id_txt = 'id_message_'.(int)$msgcont['nr'];
							}

							$ec=errorCol($errdeg);
							$igr='neutral';
							$igy='neutral';
							$igg='neutral';
							if($ec['st'] > 0)
							{
								if($ec['st'] == 1)		// success	-> green
								{
									$sttlg='1';
									$igg='green';
								}
								else if($ec['st'] == 2)	// warning	-> yellow
								{
									$sttly='1';
									$igy='yellow';
									if(!strlen((string)$mtask))
									{
										if($GLOBALS['fxpglobals']['lang'] == 1)
											$ajaxret['status_title']='<b class=orange>HINWEIS</b>';
										else
											$ajaxret['status_title']='<b class=orange>NOTE</b>';
									}
								}
								else					// error	-> red
								{
									$sttlr='1';
									$igr='red';
									if(!strlen((string)$mtask))
									{
										if($GLOBALS['fxpglobals']['lang'] == 1)
											$ajaxret['status_title']='<b class=red>FEHLER</b>';
										else
											$ajaxret['status_title']='<b class=red>ERROR</b>';
									}
								}
							}

							$ajaxret['status_message'] .= '<tr>';
							$ajaxret['status_message'] .= '	<td nowrap align="right" valign="top" style="padding:4px;color:#777777;">'.$msgcnt.':&nbsp;</td>';
							$ajaxret['status_message'] .= '	<td nowrap align="left" valign="top" style="padding:4px;">&nbsp;<img src="'.$GLOBALS['gfxpath'].'tl_'.$igr.'.png"><img src="'.$GLOBALS['gfxpath'].'tl_'.$igy.'.png" style="margin-left:1px;margin-right:1px;"><img src="'.$GLOBALS['gfxpath'].'tl_'.$igg.'.png">&nbsp;&nbsp;<br />&nbsp;<i id="'.$msg_id_txt.'" class="s1 grey">'.$msgcont['nr'].'</i></td>';
							$ajaxret['status_message'] .= '	<td valign="top" style="padding:4px;">'.$msgcont['val'].'</td>';
							$ajaxret['status_message'] .= '</tr>';

							if(!strlen((string)$mtask))
							{
								$mtask=$msgcont['val'];
							}

							$msgcnt++;
						}
					}
					$ajaxret['status_type']=$sttlr.$sttly.$sttlg;
					$ajaxret['status_message'] .= '</table>';
					$mtl=strlen((string)$mtask);
					if($GLOBALS['fxpglobals']['swidth'])
						$mxl=floor($GLOBALS['fxpglobals']['swidth']/12);
					else
						$mxl=100;
					$qra=array('"'=>'&quot;');
					if($mtl > $mxl)
						$ajaxret['status_title'] .= ' <span tooltip="'.strtr(text_wrap($mtask,true,$mxl),$qra).'" style="cursor:help;">'.strtr(text_wrap($mtask,false,$mxl),$qra).'</span>';
					else if($mtl)
						$ajaxret['status_title'] .= ' '.strtr($mtask,$qra);
				}
				if($err && !strlen((string)$mtask) && fxIsArray($GLOBALS['fxpvars']['msgarr']))
				{
					$ajaxret['status_type']='100';
					if($GLOBALS['fxpglobals']['lang'] == 1)
					{
						$ajaxret['status_title']='<b class=red>FEHLER</b>';
						$ajaxret['status_message']='Ein nicht n&auml;her definierter Fehler ist leider aufgetreten!';
					}
					else
					{
						$ajaxret['status_title']='<b class=red>ERROR</b>';
						$ajaxret['status_message']='Unfortunately a not closer specified error has occured!';
					}
				}
			}

			$tr=$ajaxret['tr'];

			// Save globals, transaction data + selects
			fxSession('g','s');
			fxSession('t','s');
			fxSession('s','s');

			$bhtml='';
			// Draw icons
			if(fxIsArray($GLOBALS['fxpvars']['ibuffer']))
			{
				ksort($GLOBALS['fxpvars']['ibuffer']);
//fxDebug($GLOBALS['fxpvars']['ibuffer'], 'Icons');
				$bhtml .= '<div style="float:right;margin-right:6px;margin-left:16px;">';
				$icnt=0;
				foreach($GLOBALS['fxpvars']['ibuffer'] as $bmn => $bmh)
				{
					$bmh=trim((string)$bmh);
					if(strlen((string)$bmh))
					{
						$ibml='';
						if($icnt)
							$ibml='margin-left:4px;';
						$bhtml .= '<div class="btn" style="position:relative;margin:0;'.$ibml.'padding:0;width:28px;height:23px;display:inline-block;overflow:none;">'.$bmh.'</div>';
						$icnt++;
					}
				}
				$bhtml .= '</div>';
			}
			// Draw buttons
			if(fxIsArray($GLOBALS['fxpvars']['bbuffer']))
			{
//fxDebug($GLOBALS['fxpvars']['bbuffer'], 'Buttons');
				$bra=array('fxfbs'=>"fxfbsb",'fxfbr'=>"fxfbrb");
				$bcnt=0;
				foreach($GLOBALS['fxpvars']['bbuffer'] as $bmn => $bmh)
				{
					$bmh=trim((string)$bmh);
					if(strlen((string)$bmh))
					{
						$bhtml .= strtr($bmh,$bra);
						$bcnt++;
					}
				}
			}

			// Get transaction path
			$fxheader='';
			if(!strlen((string)$tpath))
			{
				$fxheader='Login';
				if($uid === 'logout')
					$fxheader='Logout';
				else if(($tr != 0) && $GLOBALS['fxpglobals']['user'])
				{
					$ma=loadMenu(true);
					if(fxIsArray($ma))
					{
//fxDebug($ma, '$ma');
						$tkf='';
						$arg='';
						if(isset($_GET['repid']))
							$arg='repid='.$_GET['repid'];
						foreach($ma as $tk => $tc)
						{
							if(($tc['tr'] == $tr) && ($tc['ar'] == $arg))
							{
								$tkf=$tk;
								break;
							}
						}
						if(strlen((string)$tkf))
						{
							$fxheader=$ma[$tkf]['tx'];
							if(strlen((string)$arg))
								$arg=' arg="'.$arg.'"';
							$tpath='<div id="fxm_'.substr((string)$tkf,1).'_p" class="ttitle" tr="'.$tr.'"'.$arg.' style="float:left;font-weight:800;cursor:pointer;">'.$fxheader.'</div>';
							$no_tpath=false;
							while(true)
							{
								$tkf=substr((string)$tkf,0,-3);
								if((strlen((string)$tkf) < 3) || !isset($ma[$tkf]))
									break;
								else
									$tpath='<div id="fxm_'.substr((string)$tkf,1).'_p"  class="ttitle" style="float:left;text-decoration:underline;cursor:pointer;">'.$ma[$tkf]['tx'].'</div><div class="ttitle" style="float:left;margin:0 4px;">&gt;</div>'.$tpath;
							}
						}
					}
				}
			}
			else
			{
				$fxheader=$tpath;
				$tpath='<div class="ttitle" style="float:left;font-weight:800;">'.$tpath.'</div>';
			}
			if($no_tpath)
				$tpath='';
			if(isset($GLOBALS['set_fxheader']))
				$fxheader=$GLOBALS['set_fxheader'];

			list($usec, $sec)=explode(' ',microtime());
			$tdc1=$sec.substr((string)$usec, 1);
			$diff=$tdc1-$tdc0;

			loginProtocol($tdc0,$tdc1);
			if(!$lcnt && $GLOBALS['fxpglobals']['jscoded'])
				changeJSCPath();

			// Topscroll
			$topscroll='<div id="topscroll" style="float:left;"></div>';

			// Language
			$trlang='';
			if($GLOBALS['fxpglobals']['lang'] != $culang)
			{
				$flt=fxGetLanguageTexts('start', false);
				$trlang='<div id="trlang" style="display:none;">'.$GLOBALS['fxpglobals']['lang'].' ('.$culang.')'.$flt.'</div>';
			}
			$jslang='';
			if(($GLOBALS['fxpglobals']['lang'] != $culang) || !$otr)
			{
				$jst=fxGetLanguageTexts('js', false, true);
				$jslang='<div id="jslang" style="display:none;">'.$GLOBALS['fxpglobals']['lang'].' ('.$culang.') - '.$ntr.' ('.$otr.')'.$jst.'</div>';
			}

			// Default help file
			$mha=maskHelpFile($GLOBALS['fxpglobals']['tr'],0,$GLOBALS['fxpglobals']['lang'],true);
			$mim=$GLOBALS['gfxpath'].'im_photo'.$mha['ex'].'.png';
			$dhf_frame='<div id="dhf_frame" tooltip="'.$mha['id'].'" style="display:none;">'.$mim.'*'.$mha['mfilepath'].'</div>';

			// Company Logo
			$clogo_path=$GLOBALS['datpath'].$GLOBALS['fxpglobals']['client'].'/logo.png';
			$clogo_img='';
			if(file_exists($clogo_path))
				$clogo_img='<img id="clogo" src="'.$clogo_path.'" style="position:fixed;right:56px;bottom:56px;opacity:0.2;">';

			$html=$topscroll.$trlang.$jslang.$dhf_frame.$clogo_img.ob_get_contents();
			ob_end_clean();
			$hsize=strlen((string)$html);

			$html=str_replace('€','&euro;',$html);

			if(!isset($fxfooter))
				$fxfooter='';
			$fxfooter .= '<div id="fxfooter_timecheck" style="position:absolute;left:0;top:4px;width:100%;color:rgba(0,0,0, 0.5);font-size:10px;white-space:nowrap;">';
			$fxfooter .= 'PF: <b style="font-size:10px;">'.$ajaxret['tr'].'</b>';
			if($GLOBALS['__debug']['__DBG']&FXP_DEBUG_INF)
			{
				$fxfooter .= '&nbsp;&nbsp;/&nbsp;&nbsp;RT: '.round($tdc0,3).'-'.round($tdc1,3).' &rarr; <b style="font-size:10px;">'.round($diff,3).'s</b>';
				$fxfooter .= '&nbsp;&nbsp;/&nbsp;&nbsp;SZ: <b style="font-size:10px;">'.$hsize.' b</b>';
			}
			if(isset($GLOBALS['__scrshtmod']) && $GLOBALS['__scrshtmod'])
				$fxfooter .= '&nbsp;&nbsp;/&nbsp;&nbsp;ST: <b id="fxfooter_datetime" style="font-size:10px;">'.$GLOBALS['datetime'].'</b>';
			$GLOBALS['jsfddifctime']=0;
			$fxfooter .= '</div><div style="position:absolute;right:0;top:4px;color:rgba(255,255,255, 0.25);font-size:10px;">'.$copyright_string.'</div>';

			if(!strlen((string)$ajaxret['status_message']))
				$ajaxret['status_message']='?';

			echo($html.'*~*'.$bhtml.'*~*'.$ajaxret['tr'].'|'.$ajaxret['actions'].'|'.$tpath.'|'.$fxheader.'|'.$fxfooter.'*~*'.$ajaxret['status_type'].'*#*'.$ajaxret['status_title'].'*#*'.$ajaxret['status_message'].'*#*'.$GLOBALS['jsfddifctime']);
		break;

		case 'select':	// Get select field
			$id='';
			if(fxIsArray($_GET) && isset($_GET['id']))
				$id=trim((string)$_GET['id']);
			$sel='';
			if(fxIsArray($_GET) && isset($_GET['sel']))
				$sel=trim((string)$_GET['sel']);
			drawSelect($id,$sel);
		break;

		case 'selectvalue':	// Get selected value
			$id='';
			if(fxIsArray($_GET) && isset($_GET['id']))
				$id=trim((string)$_GET['id']);
			$val='';
			if(fxIsArray($_GET) && isset($_GET['val']))
				$val=trim((string)$_GET['val']);
			getSelectedValue($id,$val);
		break;

		case 'favorites':	// Get favorites
			drawFavorites();
		break;

		case 'search':	// Get search
			$search='';
			if(fxIsArray($_GET) && isset($_GET['search']))
				$search=trim((string)$_GET['search']);
			drawSearch($search);
		break;

		case 'sfield':	// Save field
			$frm='';
			if(fxIsArray($_GET) && isset($_GET['frm']))
				$frm=trim((string)$_GET['frm']);
			$uid='';
			if(fxIsArray($_GET) && isset($_GET['uid']))
				$uid=trim((string)$_GET['uid']);
			$fxf='';
			if(fxIsArray($_GET) && isset($_GET['fxf']))
				$fxf=trim((string)$_GET['fxf']);
			$val='';
			if(fxIsArray($_POST) && isset($_POST['val']))
				$val=$_POST['val'];
			if(strlen((string)$uid))
			{
//echo('<b>Save field:</b> tr='.$GLOBALS['fxpglobals']['tr'].', $frm='.$frm.', $uid='.$uid.', $fxf='.$fxf.', $val='.$val.'<hr>');
				fxSession('t','l','ret');
				$GLOBALS['_form']=$frm;
				// Multiselecr
				if($fxf == 'sm')
					$val=explode('|',$val);
				else if($fxf == 'cb') // Checkbox
				{
					if($val == 'true')
						$val='1';
					else if($val == 'false')
						$val=false;
				}
				if(($fxf == 'cb') && !$val)	// Unmark checkbox
					fxSession('t','d',$uid,$val);
				else
					fxSession('t','w',$uid,$val);
			}
		break;

		case 'sfields':	// Save fields
			$frm='';
			if(fxIsArray($_GET) && isset($_GET['frm']))
				$frm=trim((string)$_GET['frm']);
			if(fxIsArray($_POST))
			{
				fxSession('t','l','ret');
				$GLOBALS['_form']=$frm;
				foreach($_POST as $uid => $val)
				{
					if(fxIsArray($val))
					{
						foreach($val as $idx1 => $val1)
						{
							$uid1=$uid.'['.$idx1.']';
							if(fxIsArray($val1))
							{
								foreach($val1 as $idx2 => $val2)
								{
									$uid2 = $uid1.'['.$idx2.']';
									$fxf=substr((string)$val2,0,2);
									$sval=substr((string)$val2,2);
									if($fxf == 'in') // Integer
										$sval=(int)$sval;
									else if($fxf == 'fl') // Float
										$sval=(float)$sval;
									else if($fxf == 'cb') // Checkbox
									{
										if($sval == 'true')
											$sval='1';
										else if($sval == 'false')
											$sval=false;
									}
//echo('<b>Save fields:</b> tr='.$GLOBALS['fxpglobals']['tr'].', $frm='.$frm.', $uid2='.$uid2.', $fxf='.$fxf.', $sval='.$sval.'<br />');
									if(($fxf == 'un') || (($fxf == 'cb') && !$sval))	// Unset field or unmark checkbox
										fxSession('t','u',$uid2,$sval);
									else
										fxSession('t','p',$uid2,$sval);
								}
							}
							else
							{
								$fxf=substr((string)$val1,0,2);
								$sval=substr((string)$val1,2);
								if($fxf == 'in') // Integer
									$sval=(int)$sval;
								else if($fxf == 'fl') // Float
									$sval=(float)$sval;
								else if($fxf == 'cb') // Checkbox
								{
									if($sval == 'true')
										$sval='1';
									else if($sval == 'false')
										$sval=false;
								}
//echo('<b>Save fields:</b> tr='.$GLOBALS['fxpglobals']['tr'].', $frm='.$frm.', $uid1='.$uid1.', $fxf='.$fxf.', $sval='.$sval.'<br />');
								if(($fxf == 'un') || (($fxf == 'cb') && !$sval))	// Unset field or unmark checkbox
									fxSession('t','u',$uid1,$sval);
								else
									fxSession('t','p',$uid1,$sval);
							}
						}
					}
					else
					{
						$fxf=substr((string)$val,0,2);
						$sval=substr((string)$val,2);
						if($fxf == 'in') // Integer
							$sval=(int)$sval;
						else if($fxf == 'fl') // Float
							$sval=(float)$sval;
						else if($fxf == 'cb') // Checkbox
						{
							if($sval == 'true')
								$sval='1';
							else if($sval == 'false')
								$sval=false;
						}
//echo('<b>Save fields:</b> tr='.$GLOBALS['fxpglobals']['tr'].', $frm='.$frm.', $uid='.$uid.', $fxf='.$fxf.', $sval='.$sval.'<br />');
						if(($fxf == 'un') || (($fxf == 'cb') && !$sval))	// Unset field or unmark checkbox
							fxSession('t','u',$uid,$sval);
						else
							fxSession('t','p',$uid,$sval);
					}
				}
				fxSession('t','s');
			}
		break;


		case 'slist':	// Save list entries
			if(fxIsArray($_POST))
			{
				// Load transaction file
				$filename=fxSessionFile('t'.substr('000'.$GLOBALS['fxpglobals']['tr'],-3).'.inc');
				$tfile=fxLoad($filename);

				// Change entries
				if(strlen((string)$tfile))
				{
					$sa=array();
					foreach($_POST as $fid => $val)
					{
						if(fxIsArray($val))
						{
							foreach($val as $idx1 => $val1)
							{
								if(fxIsArray($val1))
								{
									foreach($val1 as $idx2 => $val2)
										$sa[$fid.'['.$idx1.']['.$idx2.']']=$val2;
								}
								else
									$sa[$fid.'['.$idx1.']']=$val1;
							}
						}
						else
							$sa[$fid]=$val;
					}

					foreach($sa as $fid => $val)
					{
						$s='id="'.$fid.'"';
						$p=strpos($tfile,$s);
						if($p !== false)
						{
							$te=strpos($tfile,'>',$p+strlen((string)$s));
							if($te !== false)
							{
								$te++;
								$ts=strpos($tfile,'<',$te);
								if($ts !== false)
									$tfile=substr((string)$tfile,0,$te).$val.substr((string)$tfile,$ts);
							}
						}
					}

					// Save transaction file
					fxSave($filename,$tfile,true);
				}
			}
		break;

		case 'setcb':	// Set checkboxes
			$frm='';
			if(fxIsArray($_GET) && isset($_GET['frm']))
				$frm=trim((string)$_GET['frm']);
			$cbs=array();
			if(fxIsArray($_POST) && isset($_POST['cbs']) && strlen((string)$_POST['cbs']))
				$cbs=explode('|',$_POST['cbs']);
			if(sizeof($cbs))
			{
//echo('<b>Set checkboxes:</b> tr='.$GLOBALS['fxpglobals']['tr'].', frm='.$frm.'<hr>'.$nl);
//fxDebug($cbs, '$cbs');
				fxSession('t','l','ret');
				$GLOBALS['_form']=$frm;
				foreach($cbs as $c)
				{
					$val=(int)substr((string)$c,0,1);
					$uid=substr((string)$c,1);
					if(!$val)
						fxSession('t','u',$uid,$val);
					else
						fxSession('t','p',$uid,$val);
				}
				fxSession('t','s');
			}
		break;

		case 'setnewlang':	// Switch language
			$newlang='';
			if(fxIsArray($_POST) && isset($_POST['newlang']) && strlen((string)$_POST['newlang']))
				$newlang=trim((string)$_POST['newlang']);
			if(strlen((string)$newlang))
			{
				$err=setSetting(true,1,$newlang);
				if(!$err)
				{
					fxSession('g','p','lang',$newlang);
					fxSession('g','p','reload',true);
					fxSession('g','s');
				}
			}
		break;

		case 'usersetting':	// Change user setting
			if(fxIsArray($_POST) && isset($_POST['value']) && strlen((string)$_POST['value']))
			{
				$sv=explode('|',$_POST['value']);
				$gs=false;
				foreach($sv as $s)
				{
					$fv=explode('=',$s);
					$f=strtolower((string)trim((string)$fv[0]));
					$v=trim((string)$fv[1]);

					$sql="SELECT einstellungs_id, id_feld FROM s_einstellungsdef WHERE einstellungs_index LIKE '".$f."'";
					$eia=db_values($sql);
					if(fxIsArray($eia))
					{
						$ei=(int)$eia['einstellungs_id'];
						if($ei)
						{
							$fi=(int)$eia['id_feld'];

							if($fi == 1083)	// yes/no
							{
								$v=substr((string)$v,0,1);
								if(($v == 'y') || ($v == 'j') || ($v == '1'))
								{
									$gv='Ja';
									$ev=1464;
								}
								else
								{
									$gv='Nein';
									$ev=1465;
								}
							}
							else
								$ev=$v;

							$err=setSetting(true,$ei,$ev);
							if(!$err)
							{
//								$GLOBALS['fxpglobals']['lschanged']='f='.$f.'-v='.$v.'-ei='.$ei.'-fi='.$fi.'-ev='.$ev.'-gv='.$gv;
								$GLOBALS['fxpglobals']['settings'][$f]=$gv;
								$gs=true;
							}
						}
					}
				}
				if($gs)
					fxSession('g','s');
			}
		break;

		case 'changesetting':	// Change setting (global or transaction)
			$sa=array();
			$sv=array();
			$gt=0;
			if(fxIsArray($_POST))
			{
				if(isset($_POST['str']))
					$gt=(int)$_POST['str'];
				if(isset($_POST['newsetting']) && strlen((string)$_POST['newsetting']))
					$sa=explode('|',$_POST['newsetting']);
				if(isset($_POST['value']) && strlen((string)$_POST['value']))
					$sv=explode('|',$_POST['value']);
			}
			if(sizeof($sa) && sizeof($sv) && (sizeof($sa) == sizeof($sv)))
			{
				$cf=false;
				foreach($sa as $k => $s)
				{
					if($gt > 0)
					{
						if(isset($GLOBALS['fxpglobals']['trsettings']) && fxIsArray($GLOBALS['fxpglobals']['trsettings']) && isset($GLOBALS['fxpglobals']['trsettings'][$gt]) && fxIsArray($GLOBALS['fxpglobals']['trsettings'][$gt]) && isset($GLOBALS['fxpglobals']['trsettings'][$gt][$s]))
							$GLOBALS['fxpglobals']['trsettings'][$gt][$s]=(int)$sv[$k];
					}
					else if(substr((string)$s,0,8) == 'setamenu')
					{
						$ms=(int)substr((string)$s,8);
						if($ms)
						{
							$ma=fxf_getClientFilenames($ms);
							$GLOBALS['fxpglobals']['setamenu']=$ma['n'];
						}
						else
							$GLOBALS['fxpglobals']['setamenu']='';
					}
					else if(isset($GLOBALS['fxpglobals'][$s]))
					{
						if($s == 'setfilter')
						{
							$cf=true;
							$GLOBALS['fxpglobals'][$s]=(int)$sv[$k];
						}
						else if($s == 'afilter')
						{
							$cf=true;
							$GLOBALS['fxpglobals'][$s]=trim((string)$sv[$k]);
							$fa=getFilters(false);
							eval($fa['setting']);
						}
						else
							$GLOBALS['fxpglobals'][$s]=(int)$sv[$k];
					}
				}
				fxSession('g','s');

				if($cf)
				{
					$ft=db_value("SELECT zeitstempel AS mz FROM teinstellung WHERE benutzer_id=".$GLOBALS['fxpglobals']['user']." AND trans_id=223 AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
					if(strlen((string)$ft))
					{
						$sql="UPDATE teinstellung SET zeitstempel='".$GLOBALS['datetime']."' WHERE benutzer_id=".$GLOBALS['fxpglobals']['user']." AND trans_id=223 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
						@db_query($sql);
					}
				}
			}
		break;

		case 'matrix':
			// Projekt = Typ | ID | Nummer | Name | Startdatum | Enddatum | Aufwand-Soll | Status | Open
			// Meilensteine = Projektnummer | Name | Punkt | Puffer
			// Abhängigkeiten = Projektnummer A | Projektnummer B | Typ | Minimumpuffer

			// Umgebung = Startdatum | Enddatum | Zoom | TopScroll | LeftScroll | Strukturbreite | Detailbreite | Ganttbreite | Ressourcenhöhe | Prefix

			$max_undos=25;
			$md=0;
			$mfn='';
			$c=0;
			$p=0;
			if(fxIsArray($_GET) && isset($_GET['md']) && isset($_GET['mfn']) && isset($_GET['c']) && isset($_GET['p']))
			{
				$md=(int)$_GET['md'];
				$mfn=trim((string)$_GET['mfn']);
				$c=(int)$_GET['c'];
				$p=(int)$_GET['p'];
			}
			if(!strlen((string)$mfn))
				$mfn = '*';

			if($md > 0)
			{
				$GLOBALS['fxpglobals']['client']=$c;
				$GLOBALS['fxpglobals']['person']=$p;

				$sn='t189-';
				if(substr((string)$mfn,0,1) == '*')
					$sn .= 'gdu';
				else
					$sn .= 'gdt';
				$sn .= '.txt';
				$filename=fxSessionFile($sn);

				$pf='';
				if(file_exists($filename))
				{
					$fp=@fopen($filename, 'r+');
					if($fp)
					{
						$pf=fread($fp, filesize($filename));
						fclose($fp);
					}
				}
				if(strlen((string)$pf))
					$pa=explode('*<>*', $pf);
				else
					$pa=array();

				switch($md)
				{
					case 1:	// Load
						$pd='';
						if(substr((string)$mfn,0,1) == '*')
						{
							$sp = (int)substr((string)$mfn,1);
							if(isset($pa[$sp]))
								$pd = $pa[$sp];
							else
								$pd = $pa[0];
							$pd = strtr($pd, array('€'=>'&euro;'));
							$pd .= '*~*'.sizeof($pa);
						}
						echo($pd);
					break;

					case 2: // Save
						$txt = '';

						// ...Project data
						if(fxIsArray($_POST) && isset($_POST['pa']) && fxIsArray($_POST['pa']) && sizeof($_POST['pa']))
						{
							$pt='';
							foreach($_POST['pa'] as $p)
							{
								if(strlen((string)$pt))
									$pt .= '*#*';
								$pt .= $p;
							}
							$txt .= $pt;
						}

						$txt .= '*~*';

						// ...Dependency data
						$dt='';
						if(fxIsArray($_POST) && isset($_POST['da']) && fxIsArray($_POST['da']) && sizeof($_POST['da']))
						{
							foreach($_POST['da'] as $d)
							{
								if(strlen((string)$dt))
									$dt .= '*#*';
								$dt .= $d;
							}
							$txt .= $dt;
						}

						$txt .= '*~*';

						// ...Environment
						if(fxIsArray($_POST) && isset($_POST['env']) && strlen((string)$_POST['env']))
							$txt .= trim((string)$_POST['env']);

						$ac=1;
						if(substr((string)$mfn,0,1) == '*')
						{
							if(sizeof($pa))
							{
								foreach($pa as $p)
								{
									$txt .= '*<>*';
									$txt .= $p;
									$ac++;
									if($ac > $max_undos)
										break;
								}
							}
						}
						$sca=array("%u20AC"=>'€', "<plus>"=>'+');
						fxSave($filename, strtr($txt,$sca));

						// Save environment
						if(fxIsArray($_POST) && isset($_POST['env']) && strlen((string)$_POST['env']))
						{
							$en='t189-env.txt';
							$efilename=fxSessionFile($en);
							fxSave($efilename, trim((string)$_POST['env']));
						}

						echo($ac);
					break;
				}
			}
		break;
	}
}
?>