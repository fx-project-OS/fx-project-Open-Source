<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : detail.inc                                                   //
// Version     : 24.1                                                         //
// Begin       : 2020-08-20                                                   //
// Last Change : 2024-03-14                                                   //
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
 * Framework to execute a program function, i.e. load and display masks, do basic validation etc.
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$GLOBALS['__loaded_'.basename(__FILE__)]=true;
//$GLOBALS['__debug']['__DBG']=FXP_DEBUG_ALL;
//$GLOBALS['__debug']['__DBG'] |= FXP_DEBUG_FIL;	//|FXP_DEBUG_TIM|FXP_DEBUG_MEM;
//$GLOBALS['__debug']['debugmode']=FXP_DEBUG_FCT;


////////////////////////////////////////////////////////////////////////////////
// VARIABLES
////////////////////////////////////////////////////////////////////////////////
$GLOBALS['fxpvars']['msgarr']=array();
$GLOBALS['fxpselects']['sqlresults']='';
$GLOBALS['fxptdata']['mskislist']=false;
$GLOBALS['fxptdata']['lvalues']=array();

// Error
$err=false;

// Debug setting: Start runtime/memmory
fxf_dbgRuntime(0, 'black', 'START');
fxf_dbgMemory('black', 'START');

// History timestamp
$_history_timestamp=$GLOBALS['datetime'];

// Remember converted values
$_already_converted=array();

////////////////////////////////////////////////////////////////////////////////
// PROGRAM FUNCTION DATA
////////////////////////////////////////////////////////////////////////////////
$tr=(int)$tr;

// Program and special program function directory
$_spa=array('pfd'=>'prgdir', 'sfd'=>'sppdir');
foreach($_spa as $_spv => $_spd)
{
	${'_'.$_spv}='';
	if(fxIsArray($GLOBALS[$_spd]) && isset($GLOBALS[$_spd][$tr]))
	{
		${'_'.$_spv}=$GLOBALS[$_spd][$tr];
		if(substr((string)${'_'.$_spv},-1) != '/')
			${'_'.$_spv} .= '/';
	}
//fxDebug(${'_'.$_spv},'$_'.$_spv, 0);
}

// Read data from database for this program function (like masks, conditions, validations, parameters)
$_pfda=array();
if($tr)
{
	$_sql  = "SELECT tm.zeile, tm.nummer, m.name_maske, m.auswahlmaske, tm.bedingung, tm.validierung, t.trans_art, t.param";
	$_sql .= " FROM trans_maske tm, masken m, trans t";
	$_sql .= " WHERE m.nummer=tm.nummer AND tm.transakt_id=".$tr." AND tm.transakt_id=t.transakt_id AND (m.mandanten_id IS NULL OR m.mandanten_id IN (0, ".$GLOBALS['fxpglobals']['client']."))";
	$_sql .= " ORDER BY tm.zeile";
	$_pfda=db_values($_sql, '*');
//fxDebug($_pfda,'$_pfda:<br />'.$_sql, 0);
}

// Set parameters by evaluating php string (e.g. '$Personentyp=275')
if(fxIsArray($_pfda) && fxIsArray($_pfda[0]) && isset($_pfda[0]['param']) && strlen((string)$_pfda[0]['param']))
{
	$_tmp=strtr($_pfda[0]['param'], array('&quot;'=>'"', '\\'=>''));
	if(substr((string)$_tmp, strlen((string)$_tmp)-1) != ';')
		$_tmp .= ';';
	eval($_tmp);
}

// Retrieve backup variables
if(fxIsArray($GLOBALS['fxptdata']['backup']) && fxIsArray($GLOBALS['fxptdata']['backup']))
	extract($GLOBALS['fxptdata']['backup']);

// Debug setting: Display Superglobals?
if($GLOBALS['__debug']['__DBG']&FXP_DEBUG_PST)
{
	echo('<div class="dbgd"><div style="float:left;margin-right:8px;"><img src="'.$GLOBALS['gfxpath'].'net_b_16x16.png"></div>');
	echo('<div class="lightergrey" style="float:left;width:320px;font-size:smaller;">'.__FILE__.' ('.__LINE__.')</div>');
	echo('<div class="black" style="float:left;width:240px;font-weight:bold;">SUPERGLOBALS</div><br />'.$GLOBALS['nl']);
	echo('<div style="margin-left:24px;margin-top:6px;">'.$GLOBALS['nl']);
	fxDebug($_GET,'$_GET', 0); fxDebug($_POST,'$_POST', 0);
	echo('</div>'.$GLOBALS['nl']);
	echo('</div>'.$GLOBALS['nl']);
}

// 1: Optional default program and special program function includes "*_ini.inc" or "*_ini_usr.inc"
$dbii='';
if($tr)
{
	$_ifsa=array('ini', 'ini_usr');
	foreach($_spa as $_spv => $_spd)
	{
		if(strlen((string)${'_'.$_spv}))
		{
			foreach($_ifsa as $_ifs)
			{
				$inc=${'_'.$_spv}.substr('000'.$tr,-3).'_'.$_ifs.'.inc';
				fxf_dbgFiles($inc);
				if(file_exists($inc))
				{
					require($inc);
					if(strlen((string)$dbii))
						$dbii .= ' + ';
					$dbii .= $inc;
				}
			}
		}
	}
}

// Additional or alias program and special program function directories $_pfaa (can be set in "*_ini.inc" or "*_ini_usr.inc")?
$_pfa=array();
$_pfi=array(substr('000'.$tr,-3)=>true);
foreach($_spa as $_spv => $_spd)
{
	if(strlen((string)${'_'.$_spv}))
		$_pfa[]=${'_'.$_spv};
}
if(fxIsArray($_apfa))
{
	foreach($_spa as $_spv => $_spd)
	{
		if(fxIsArray($GLOBALS[$_spd]))
		{
			foreach($_apfa as $atr)
			{
				$_pfi[substr('000'.$atr,-3)]=true;
				if(isset($GLOBALS[$_spd][$atr]))
				{
					$_apfd=$GLOBALS[$_spd][$atr];
					if(substr((string)$_apfd,-1) != '/')
						$_apfd .= '/';
					$_pfa[]=$_apfd;
				}
			}
		}
	}
}
// Global program and special program function directory
foreach($_spa as $_spv => $_spd)
{
	if(fxIsArray($GLOBALS[$_spd]) && isset($GLOBALS[$_spd][0]))
	{
		$_gpfd=$GLOBALS[$_spd][0];
		if(substr((string)$_gpfd,-1) != '/')
			$_gpfd .= '/';
		$_pfa[]=$_gpfd;
	}
}
$_pfi['000']=true;
fxDebug($_pfa,'$_pfa', 3);

// Retrieve person id
$ppersonen_id=0;
if(fxIsArray($_GET) && $_GET['Person_alias'])
{
	$ppersonen_id=(int)$_GET['Person_alias'];
	$_POST=array('Person_alias'=>$ppersonen_id, 'Person_waehlen'=>true, 'sf'.$tr.'_set'=>382, 'sf382_Person_alias'=>"s", 'tr'=>$tr);
}

// Debug: Runtime/Memory?
fxf_dbgRuntime(2, 'grey', 'PRE-INIT', $dbii);
fxf_dbgMemory('grey', 'PRE_INIT', $dbii);

// 2. Optional special includes set in $ini_inc
if(isset($ini_inc) && strlen((string)$ini_inc))
{
	foreach($_spa as $_spv => $_spd)
	{
		if(strlen((string)${'_'.$_spv}))
		{
			$inc=${'_'.$_spv}.strtolower((string)$ini_inc);
			fxf_dbgFiles($inc, true);
			if(file_exists($inc))
			{
				require($inc);
				fxf_dbgRuntime(2, 'grey', 'SET', $inc);
				fxf_dbgMemory('grey', 'SET', $inc);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// PROGRAM FUNCTION: VALIDATION/DATABASE ROUTINES
////////////////////////////////////////////////////////////////////////////////
$dbwerte=array();
if(fxIsArray($_pfda))
{
	// Debug: Runtime?
	fxf_dbgRuntime(1, 'lightgreen', 'START SUB: DB');
	$_RBA=false;

	// 3. Include all existing mask initialization files "*_ini.inc" or "*_ini_usr.inc"
	$_ifsa=array('ini', 'ini_usr');
	foreach($_pfda as $_cnt => $_pfma)
	{
		foreach($_pfa as $_pfc)
		{
			foreach($_ifsa as $_ifs)
			{
				$inc=$_pfc.strtolower((string)$_pfma['name_maske'].'_'.$_ifs.'.inc');
				fxf_dbgFiles($inc, true);
				if(file_exists($inc))
				{
					require($inc);
					break;
				}
			}
		}
	}

	foreach($_pfda as $_cnt => $_pfma)
	{
		if(!$_pfma['nummer'])
			$GLOBALS['_maskcounter']++;

		$_pfmanr=(string)trim((string)$_pfma['nummer']);
		if(!is_array($GLOBALS['fx_mfields'][$_pfmanr]))
			$_pfmanr=(int)$_pfmanr;

		// Check if mask should be displayed
		$_pfmanv='maske'.$_pfmanr.'_nv';
		if($GLOBALS[$_pfmanv])
		{
			$inc_check[$_pfmanr]=false;
			$note='<b class="red">NO</b> &rarr; <span class="grey">Not visible set: $GLOBALS[\''.$_pfmanv.'\'] == '.$GLOBALS[$_pfmanv].'</span> &rarr; <span class="grey smaller">$inc_check['.$_pfmanr.']=false</span>';
		}
		else if(isset($_pfma['bedingung']) && ($_pfma['bedingung'] !== NULL) && strlen((string)$_pfma['bedingung']))
		{
			$_bedingung=str_replace('&quot;', '"', $_pfma['bedingung']);
			$note=' &rarr; <span class="grey">eval(\''.fxHtmlEncode($_bedingung).'\') = ';
			$_bedingung='if('.$_bedingung.') { $inc_check[$_pfmanr]=true; } else { $inc_check[$_pfmanr]=false; }';
			eval($_bedingung);
			if($inc_check[$_pfmanr])
				$note='<b class="green">YES</b>'.$note.'true</span> &rarr; <span class="grey smaller">$inc_check['.$_pfmanr.']=true</span>';
			else
				$note='<b class="red">NO</b>'.$note.'false</span> &rarr; <span class="grey smaller">$inc_check['.$_pfmanr.']=false</span>';
		}
		else
		{
			$inc_check[$_pfmanr]=true;
			$note='<b class="green">YES</b> &rarr; <span class="grey smaller">$inc_check['.$_pfmanr.']=true</span>';
		}
		$_fia[$_cnt]['maske']=$inc_check[$_pfmanr];
		// Debug: Frame?
		fxf_dbgType(FXP_DEBUG_FRM, '#'.$_cnt.': Display mask '.$_pfmanr.' (= &quot;'.$_pfma['name_maske'].'&quot;)?', $note);

		// Check if default validation should be executed
		sessionMask($_pfmanr);
		$val_disp_mask=true;
		if(!$inc_check[(int)$_pfmanr] && !$inc_check[(string)$_pfmanr])
			$val_disp_mask=false;
		$_pfmanovalid='maske'.$_pfmanr.'_novalid';
		$valid=true;

		// No validation?
		if(fxIsArray($_POST) && isset($_POST['Reload']) && $_POST['Reload'])
		{
			$valid=false;
			if(isset($db_update))
				unset($db_update);
			if($GLOBALS['__debug']['__DBG'] == FXP_DEBUG_VAL)
				echo('<b class="grey">No auto validation, because RELOAD!</b><br />');
		}
		else if(isset($GLOBALS[$_pfmanovalid]) && $GLOBALS[$_pfmanovalid])
		{
			$valid=false;
			if($GLOBALS['__debug']['__DBG'] == FXP_DEBUG_VAL)
				echo('<b class="grey">No auto validation, because '.$_pfmanovalid.' was set in program!</b><br />');
		}
		else if(!fxIsArray($GLOBALS['fx_mfields']))
		{
			$valid=false;
			if($GLOBALS['__debug']['__DBG'] == FXP_DEBUG_VAL)
				echo('<b class="grey">No auto validation, because fx_mfields is not or an empty array!</b><br />');
		}
		else if(!fxIsArray($GLOBALS['fx_mfields'][$_pfmanr]))
		{
			$valid=false;
			if($GLOBALS['__debug']['__DBG'] == FXP_DEBUG_VAL)
				echo('<b class="grey">No auto validation, because fx_mfields['.$_pfmanr.'] is not or an empty array!</b><br />');
		}
		else if(isset($force_valid) && $force_valid)
		{
			if($GLOBALS['__debug']['__DBG'] == FXP_DEBUG_VAL)
				echo('<b class="green">FORCE: Always auto validate...</b><br />');
		}
		else if(!$_pfma['validierung'] || !$val_disp_mask || !$db_update || ($aktion == FXP_DISPLAY) || ($aktion == FXP_DELETE))
		{
			$valid=false;
			if($GLOBALS['__debug']['__DBG'] == FXP_DEBUG_VAL)
			{
				if(!$_pfma['validierung'])
					echo('<b class="grey">No auto validation, because of database entry!</b><br />');
				else if(!$val_disp_mask)
					echo('<b class="grey">No auto validation, because mask #'.$_pfmanr.' will not be displayed!</b><br />');
				else if($aktion == FXP_DISPLAY)
					echo('<b class="grey">No auto validation, because of mode: DISPLAY!</b><br />');
				else if($aktion == FXP_DELETE)
					echo('<b class="grey">No auto validation, because of mode: DELETE!</b><br />');
				else
					echo('<b class="red">No auto validation, because $db_update is not set!</b><br />');
			}
		}

		if($valid)
		{
			if(isset($_POST['FeldTyp']) && !isset($_POST['Feldtyp']))
			{
				$_POST['Feldtyp']=$_POST['FeldTyp'];
				$Feldtyp=$_POST['Feldtyp'];
			}
			if($_POST['Personen_ID'] && !$Personen_ID)
				$Personen_ID=$_POST['Personen_ID'];

			foreach($GLOBALS['fx_mfields'][$_pfmanr] as $_g => $_mskfld)
			{
				$_v_feldname=trim((string)$_mskfld['feldname']);
				if(strlen((string)$_v_feldname) < 2)
					continue;
				$_v_lower_feldname=strtolower((string)$_v_feldname);
				$_v_anzeigeart=(int)$_mskfld['anzeigeart'];

				// Don't validate literals or "not visible" fields
				if(${$_v_feldname.'_nv'} || ($_v_anzeigeart < 5))
				{
					if($GLOBALS['__debug']['__DBG'] == FXP_DEBUG_VAL)
					{
						if($_v_anzeigeart < 5)
							echo('<i class="lightgrey">(No auto validation, because of field type LITERAL for &quot;'.$_v_feldname.'&quot;!)</i><br />');
						else
							echo('<i class="lightgrey">(No auto validation, because field &quot;'.$_v_feldname.'&quot; is not visible!)</i><br />');
					}
					continue;
				}
				// Only check submitted fields (POST/GET) except checkboxes
				else if(($_v_anzeigeart != 11) && !isset($_POST[$_v_feldname]) && !isset($_GET[$_v_feldname]))
				{
					if($GLOBALS['__debug']['__DBG'] == FXP_DEBUG_VAL)
						echo('<i class="lightgrey">(No auto validation, because field &quot;'.$_v_feldname.'&quot; was not submitted... Set to NULL!)</i><br />');
					${$_v_feldname}='';
					$maskenfeldwerte[$_v_lower_feldname]='';
					$dbwerte[$_v_lower_feldname]='NULL';
					continue;
				}

				$_v_feldtyp=(int)$_mskfld['feldtyp'];
				if(($_v_feldtyp == 1) || ($_v_feldtyp == 3) ||  ($_v_feldtyp == 8) || ($_v_feldtyp == 15))
					$_v_convert=true;
				else
					$_v_convert=false;
				if($_v_convert || ($_v_feldtyp == 7) || ($_v_feldtyp == 9) || ($_v_feldtyp == 10) || ($_v_feldtyp == 13))
					$_v_textfeld=true;
				else
					$_v_textfeld=false;

				// If the field value is an array (e.g. Multi-selects), no trim((string))
				if(is_array($_POST[$_v_feldname]))
					$val_wert=$_POST[$_v_feldname];
				else if(is_array($GLOBALS[$_v_feldname]))
					$val_wert=$GLOBALS[$_v_feldname];
				else if(isset($_POST[$_v_feldname]))
					$val_wert=trim((string)$_POST[$_v_feldname]);
				else if(isset($GLOBALS[$_v_feldname]))
					$val_wert=trim((string)$GLOBALS[$_v_feldname]);
				else
					$val_wert='';

				$fnm=$_v_feldname.'_muss';
				if(isset($GLOBALS[$fnm]))
					$rv_muss=$GLOBALS[$fnm];
				else
					$rv_muss=$_mskfld['muss'];
				$db_wert=$val_wert;
				if($GLOBALS['__debug']['__DBG'] == FXP_DEBUG_VAL)
				{
					if(!is_array($val_wert))
						$vwhtml=fxHtmlEncode($val_wert);
					else
						$vwhtml='-ARRAY-';
					echo('<div style="border:1px solid black;border-radius:6px;padding:3px;margin-top:2px;"><b>'.$_v_feldname.'</b> &rarr; Value: &quot;<font class="blue">'.$vwhtml.'</font>&quot;, Display Type: [<font class="blue">'.$_v_anzeigeart.' = '.$anzart_defines[$_v_anzeigeart].'</font>], Field Type: [<font class="blue">'.$_v_feldtyp.' = '.$GLOBALS['_ftypes'][$_v_feldtyp].'</font>], Mandatory: ['.($rv_muss?'<font class="blue">YES</font>':'<i class="grey">NO</i>').']...<br />');
				}
				// Validation: Only execute for certain display types and fetch the rest
				// ...Selects
				if($_v_anzeigeart == 8)
				{
					if($GLOBALS['__debug']['__DBG'] == FXP_DEBUG_VAL)
						echo(' &rarr; Auto validate [<b>'.$GLOBALS['anzart_disptxt'][$_v_anzeigeart].'</b>] field... ');
					// No item selected, but it's mandatory
					if($rv_muss && !$val_wert)
					{
						if($GLOBALS['__debug']['__DBG'] == FXP_DEBUG_VAL)
							echo('<b class="red">ERROR #789</b> An item must be selected in the field &quot;'.$_v_feldname.'&quot;!<br />');
						error_msg(789, false, $_v_feldname);
						$err=true;
					}
					else if($GLOBALS['__debug']['__DBG'] == FXP_DEBUG_VAL)
						echo('<b class="green">OK</b> Item &quot;'.$val_wert.'&quot; selected.<br />');
				}
				// ...Files
				else if($_v_anzeigeart == 16)
				{
					if($GLOBALS['__debug']['__DBG'] == FXP_DEBUG_VAL)
						echo(' &rarr; Auto validate [<b>'.$GLOBALS['anzart_disptxt'][$_v_anzeigeart].'</b>] field... ');
					// No file selected or entered, but it's mandatory
					if($rv_muss && (!$val_wert || ($val_wert == 'none')))
					{
						if($GLOBALS['__debug']['__DBG'] == FXP_DEBUG_VAL)
							echo('<b class="red">ERROR #925</b> A valid file name must be selected/entered in the field &quot;'.$_v_feldname.'&quot;!<br />');
						error_msg(925, false, $_v_feldname);
						$err=true;
					}
					else if($GLOBALS['__debug']['__DBG'] == FXP_DEBUG_VAL)
						echo('<b class="green">OK</b> File &quot;'.$val_wert.'&quot; selected.<br />');
				}
				// ...Texts, Textareas, Checkboxes + Sums
				else if(($_v_anzeigeart == 6) || ($_v_anzeigeart == 7) || ($_v_anzeigeart == 11) || ($_v_anzeigeart == 14))
				{
					$errv=false;
					if($GLOBALS['__debug']['__DBG'] == FXP_DEBUG_VAL)
						echo(' &rarr; Auto validate [<b>'.$GLOBALS['anzart_disptxt'][$_v_anzeigeart].'</b>] field... ');
					$neu=validate_entry($val_wert, $errv, $GLOBALS['fx_mfields'][$_pfmanr][$_g]);
					if($errv != false)
					{
						if($GLOBALS['__debug']['__DBG'] == FXP_DEBUG_VAL)
						{
							$mld_err=meldung($errv, false, $_v_feldname);
							if(is_array($mld_err))
								$mld_err=$mld_err['tabwert'];
							echo('<b class="red">ERROR #'.$errv.'</b> '.$mld_err.'<br />');
						}
						error_msg($errv, false, $_v_feldname);
						$err=true;
					}
					else if($GLOBALS['__debug']['__DBG'] == FXP_DEBUG_VAL)
						echo('<b class="green">OK</b> Validated value is &quot;'.$neu.'&quot;.<br />');
					$db_wert=$neu;
					$val_wert=get_entry($neu, $GLOBALS['fx_mfields'][$_pfmanr][$_g]);
				}
				else if($GLOBALS['__debug']['__DBG'] == FXP_DEBUG_VAL)
					echo(' -> <i class="grey">No auto validation!</i><br />');
				if($GLOBALS['__debug']['__DBG'] == FXP_DEBUG_VAL)
					echo('</div>');

				if($_v_convert)
				{
					if(!is_array($_already_converted[$_v_feldname]))
					{
						$val_wert=convert_string($val_wert, 'todb');
						$db_wert=convert_string($db_wert, 'todb');

						$_already_converted[$_v_feldname]=array('val'=>$val_wert, 'db'=>$db_wert);
					}
					else
					{
						$val_wert=$_already_converted[$_v_feldname]['val'];
						$db_wert=$_already_converted[$_v_feldname]['db'];
					}
				}
				${$_v_feldname}=$val_wert;
				$maskenfeldwerte[$_v_lower_feldname]=$db_wert;
				if(!is_string($db_wert) || !strlen((string)$db_wert))
					$dbwerte[$_v_lower_feldname]='NULL';
				else if($_v_textfeld)
					$dbwerte[$_v_lower_feldname]="'".$db_wert."'";
				else
					$dbwerte[$_v_lower_feldname]=extractNumberFromString($db_wert);
			}
		}

		// User defined validation with cross-check
		if(isset($db_update) && $db_update)
		{
			// Validation only necessary for action modes "Create" or "Change"
			if(($aktion == FXP_CREATE) || ($aktion == FXP_CHANGE) || (isset($GLOBALS['force_val']) && $GLOBALS['force_val']))
			{
				$note='<b class="green">YES</b> &larr; <span class="grey smaller">$db_update == ['.$db_update.'] + $aktion == ['.$aktion.'] + $force_val == ['.$GLOBALS['force_val'].']</span>';

				// 4. Include existing mask validation file "*_val.inc" or "*_val_usr.inc"
				$_vfsa=array('val', 'val_usr');
				foreach($_pfa as $_pfc)
				{
					foreach($_vfsa as $_vfs)
					{
						$inc=$_pfc.strtolower((string)$_pfma['name_maske'].'_'.$_vfs.'.inc');
						fxf_dbgFiles($inc);
						if(file_exists($inc))
						{
							require($inc);
							fxf_dbgRuntime(2, 'green', 'VALIDATE', $inc);
							fxf_dbgMemory('green', 'VALIDATE', $inc);
							if($err)
								break;
						}
					}
				}
				unset($val_zeilennummer);
			}
			else
				$note='<b class="red">NO</b> &larr; <span class="grey smaller">$db_update == ['.$db_update.'], but $aktion == ['.$aktion.']</span>';

			// Execute database files if there is no validation error
			if(!$err && !$_POST['Button_Check'] && isset($db_update) && $db_update)
			{
				if(!$_RBA)
				{
					db_rollback('start'); // Set database transaction start
					$_RBA=true;
					// Debug: Frame/File?
					fxf_dbgType(FXP_DEBUG_FRM|FXP_DEBUG_FIL, '#'.$_cnt.': <b>Database</b> &rarr; Transaction start marker set.');
				}

				// 5. Include existing mask database file "*_db.inc" or "*_db_usr.inc"
				$_dfsa=array('db', 'db_usr');
				foreach($_pfa as $_pfc)
				{
					foreach($_dfsa as $_dfs)
					{
						$inc=$_pfc.strtolower((string)$_pfma['name_maske'].'_'.$_dfs.'.inc');
						fxf_dbgFiles($inc);
						if(file_exists($inc))
						{
							require($inc);
							fxf_dbgRuntime(2, 'green', 'DATABASE', $inc);
							fxf_dbgMemory('green', 'DATABASE', $inc);
							if($err)
								break;
						}
					}
				}
			}
		}
		else
		{
			$note='<b class="red">NO</b> &larr; <span class="grey smaller">$db_update ';
			if(!isset($db_update))
				$note .= 'is not set';
			else if(($db_update === NULL) || !strlen((string)$db_update))
				$note .= 'is empty';
			else
				$note .= '== ['.$db_update.']';
			$note .= '</span>';
		}
		// Debug: Frame?
		fxf_dbgType(FXP_DEBUG_FRM, '#'.$_cnt.': Validate mask '.$_pfmanr.' (= &quot;'.$_pfma['name_maske'].'&quot;)?', $note);
		$GLOBALS['_maskcounter']++;
	}

	// 6. Optional special database includes
	if($tr && !$err && !$_POST['Button_Check'] && isset($db_update) && $db_update)
	{
		$_dfsa=array('db', 'db_usr');
		foreach($_pfa as $_pfc)
		{
			foreach($_pfi as $itr => $iex)
			{
				if($iex)
				{
					foreach($_dfsa as $_dfs)
					{
						$inc=$_pfc.$itr.'_'.$_dfs.'.inc';
						fxf_dbgFiles($inc);
						if(file_exists($inc))
						{
							if(!$_RBA)
							{
								db_rollback('start'); // Set database transaction start
								$_RBA=true;
								// Debug: Frame/File?
								fxf_dbgType(FXP_DEBUG_FRM|FXP_DEBUG_FIL, '#'.$_cnt.': <b>Database</b> &rarr; Transaction start marker set.');
							}

							require($inc);
							fxf_dbgRuntime(2, 'green', 'DATABASE', $inc);
							fxf_dbgMemory('green', 'DATABASE', $inc);
							if($err)
								break;
						}
					}
				}
			}
			if($err)
				break;
		}
	}

	// Rollback all database changes if there was an error, else commit them
	if($_RBA)
	{
		if($err)
		{
			db_rollback('abbruch');
			// Debug: Frame/File?
			fxf_dbgType(FXP_DEBUG_FRM|FXP_DEBUG_FIL, '#'.$_cnt.': <b>Database</b> &rarr; <span class="red">Rollback!</span>');
		}
		else
		{
			db_rollback('ok');
			// Debug: Frame/File?
			fxf_dbgType(FXP_DEBUG_FRM|FXP_DEBUG_FIL, '#'.$_cnt.': <b>Database</b> &rarr; <span class="green">Commit!</span>');
		}
	}

	// Debug: Runtime/Memory?
	fxf_dbgRuntime(3, 'darkgreen', 'TOTAL DB');
	fxf_dbgMemory('darkgreen', 'TOTAL DB');
}

////////////////////////////////////////////////////////////////////////////////
// PROGRAM FUNCTION: MASK ROUTINES
////////////////////////////////////////////////////////////////////////////////
if(fxIsArray($_pfda))
{
//fxDebug($_fia,'$_fia', 0);
	fxf_dbgRuntime(1, 'lightblue', 'START SUB: MASKS');
	$GLOBALS['_maskcounter']=1;

	// Set error for click on "Check"
	if(!isset($_set_noerror) && $_POST['Button_Check'])
		$err=true;

	foreach($_pfda as $_cnt => $_pfma)
	{
		$_pfmanr=(string)trim((string)$_pfma['nummer']);
		if(!is_array($GLOBALS['fx_mfields'][$_pfmanr]))
			$_pfmanr=(int)$_pfmanr;
		$GLOBALS['_masknumber']=$_pfmanr;
		// Debug: Frame/File?
		fxf_dbgType(FXP_DEBUG_FRM|FXP_DEBUG_FIL, '#'.$_cnt.': <b>$GLOBALS[\'_masknumber\']</b> &rarr; Set to ['.$GLOBALS['_masknumber'].']');

		$r_msk_n='maske'.$GLOBALS['_masknumber'].'_nv';
		if(isset($GLOBALS[$r_msk_n]))
			$r_msk_nv=$GLOBALS[$r_msk_n];
		else
			$r_msk_nv=false;

		$_imp=false;
		$note='<b class="red">NO</b>';
		if((isset($include_all_masks) && $include_all_masks) || (!$r_msk_nv && $_fia[$_cnt]['maske']))
		{
			$_imp=true;
			$note='<b class="green">YES</b>';
		}
		$note .= ' &larr; <span class="grey smaller">$include_all_masks '.(isset($include_all_masks)?(strlen((string)$include_all_masks)?'= ['.$include_all_masks.']':'is empty'):'is not set').'</span>';
		$note .= ' + <span class="grey smaller">Not visible: $r_msk_nv = ['.$r_msk_nv.']</span> + <span class="grey smaller">Parameter: \'maske\' = ['.$_fia[$_cnt]['maske'].']</span>';
		// Debug: Frame?
		fxf_dbgType(FXP_DEBUG_FRM, '#'.$_cnt.': Include mask '.$GLOBALS['_masknumber'].' (= &quot;'.$_pfma['name_maske'].'&quot;) definition file?', $note);

		if($_imp)
		{
			// 7. Include existing mask definition file "*_pre.inc" or "*_pre_usr.inc"
			$_eds=true;
			$_pfsa=array('pre', 'pre_usr');
			foreach($_pfa as $_pfc)
			{
				foreach($_pfsa as $_pfs)
				{
					$inc=$_pfc.strtolower((string)$_pfma['name_maske'].'_'.$_pfs.'.inc');
					fxf_dbgFiles($inc);
					if(file_exists($inc))
					{
						require($inc);
						fxf_dbgRuntime(2, 'blue', 'PRE', $inc);
						fxf_dbgMemory('blue', 'PRE', $inc);
						$_eds=false;
						break;
					}
				}
			}
			if($_eds && ($GLOBALS['_masknumber'] < 0))
				enable_dummies();
		}

		$_dsp=false;
		$note='<b class="red">NO</b>';
		if((!isset($display_inc) || !$display_inc || $display_all) && !$r_msk_nv && $_fia[$_cnt]['maske'])
		{
			$_dsp=true;
			$note='<b class="green">YES</b>';
		}
		$note .= ' &larr; <span class="grey smaller">$display_inc '.(isset($display_inc)?(strlen((string)$display_inc)?'= ['.$display_inc.']':'is empty'):'is not set').'</span>';
		$note .= ' + <span class="grey smaller">$display_all '.(isset($display_all)?(strlen((string)$display_all)?'= ['.$display_all.']':'is empty'):'is not set').'</span>';
		$note .= ' + <span class="grey smaller">Not visible: $r_msk_nv = ['.$r_msk_nv.']</span> + <span class="grey smaller">Parameter: \'maske\' = ['.$_fia[$_cnt]['maske'].']</span>';
		// Debug: Frame?
		fxf_dbgType(FXP_DEBUG_FRM, '#'.$_cnt.': Display mask '.$GLOBALS['_masknumber'].' (= &quot;'.$_pfma['name_maske'].'&quot;)?', $note);
		if($_dsp)
		{
			// Display mask
			$mask_html=maske($GLOBALS['_masknumber']);
//echo('<pre>'.fxHtmlEncode($mask_html).'</pre>');
			echo($mask_html);

			// 8. Include existing mask post file "*_pst.inc" or "*_pst_usr.inc"
			$_pfsa=array('pst', 'pst_usr');
			foreach($_pfa as $_pfc)
			{
				foreach($_pfsa as $_pfs)
				{
					$inc=$_pfc.strtolower((string)$_pfma['name_maske'].'_'.$_pfs.'.inc');
					fxf_dbgFiles($inc);
					if(file_exists($inc))
					{
						require($inc);
						fxf_dbgRuntime(2, 'blue', 'PST', $inc);
						fxf_dbgMemory('blue', 'PST', $inc);
						break;
					}
				}
			}
		}
		else
			$GLOBALS['_maskcounter']++;
	}

	// Debug: Runtime/Memory?
	fxf_dbgRuntime(3, 'darkblue', 'TOTAL MASKS');
	fxf_dbgMemory('darkblue', 'TOTAL MASKS');
}
else
	$GLOBALS['_maskcounter']=1;

////////////////////////////////////////////////////////////////////////////////
// PROGRAM FUNCTION: POST MASK ROUTINE
////////////////////////////////////////////////////////////////////////////////
// 9: Optional post program function includes "*_pst.inc" or "*_pst_usr.inc"
$dbpi='';
if($tr)
{
	$_pfsa=array('pst', 'pst_usr');
	foreach($_spa as $_spv => $_spd)
	{
		if(strlen((string)${'_'.$_spv}))
		{
			foreach($_pfsa as $_pfs)
			{
				$inc=${'_'.$_spv}.substr('000'.$tr,-3).'_'.$_pfs.'.inc';
				fxf_dbgFiles($inc);
				if(file_exists($inc))
				{
					require($inc);
					if(strlen((string)$dbpi))
						$dbpi .= ' + ';
					$dbpi .= $inc;
				}
			}
		}
	}
}

// Debug: Runtime/Memory?
fxf_dbgRuntime(2, 'grey', 'PST', $dbpi);
fxf_dbgMemory('grey', 'PST', $dbpi);

////////////////////////////////////////////////////////////////////////////////
// PROGRAM FUNCTION: SPECIAL MASK ROUTINE
////////////////////////////////////////////////////////////////////////////////
// 10. Optional special mask include set in $display_inc
if(isset($display_inc) && strlen((string)$display_inc))
{
	$inc=$_pfd.strtolower((string)$display_inc);
	fxf_dbgFiles($inc);
	if(file_exists($inc))
	{
		require($inc);
		fxf_dbgRuntime(2, 'grey', 'DISPLAY', $inc);
		fxf_dbgMemory('grey', 'DISPLAY', $inc);
	}
}

// Save updated settings?
if(!$err && fxIsArray($ls_rem_sql))
{
	foreach($ls_rem_sql as $ls_sql)
	{
		$ls_dberr=db_x($ls_sql, 0, false, true, false);
//fxDebug($ls_dberr,$ls_sql, 0);
	}
}

////////////////////////////////////////////////////////////////////////////////
// PROGRAM FUNCTION: JAVASCRIPT
////////////////////////////////////////////////////////////////////////////////
// 11. Optional program function javascript includes
if(($ntr > 0) && ($ntr != $otr) && (!isset($_GET['jse']) || !$_GET['jse']))
{
	$trjsi='';
	$_jfsa=array('', '_usr');
	foreach($_pfa as $_pfc)
	{
		foreach($_pfi as $itr => $iex)
		{
			if(((int)$itr > 0) && $iex)
			{
				foreach($_jfsa as $_jfs)
				{
					$itr3=substr('000'.$itr,-3);
					$inc=$_pfc.$itr3.$_jfs.'.js';
					fxf_dbgFiles($inc);
					if(file_exists($inc))
					{
						if(strlen((string)$trjsi))
							$trjsi .= '!-FXJS-!';
						$trjsi .= '#'.$itr3.$inc.'?'.filemtime($inc);
						fxf_dbgRuntime(2, 'grey', 'JAVASCRIPT', $inc);
						fxf_dbgMemory('grey', 'JAVASCRIPT', $inc);
					}

					$inc=$_pfc.$itr3.$_jfs.'_js.inc';
					fxf_dbgFiles($inc);
					if(file_exists($inc))
					{
						require($inc);
						fxf_dbgRuntime(2, 'grey', 'JAVASCRIPT', $inc);
						fxf_dbgMemory('grey', 'JAVASCRIPT', $inc);
					}
				}
			}
		}
	}
	if(strlen((string)$trjsi))
		echo('<div id="trjsi" style="display:none;">'.$trjsi.'</div>');
}

// Debug: Runtime/Memory?
fxf_dbgRuntime(4, 'black', 'TOTAL');
fxf_dbgMemory('black', 'TOTAL');
?>