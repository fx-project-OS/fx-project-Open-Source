<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : db_install.inc                                               //
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
 * Install or update fx-project
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */


$quit=false;

// Load initialization texts in selected language
if(!fxIsArray($GLOBALS['_itla']))
	$GLOBALS['_itla']=fxGetLanguageTexts('init');
//fxDebug($GLOBALS['_itla'],'$GLOBALS[\'_itla\']: fxGetLanguageTexts(\'init\')',0);

// Installation or Update?
$fxinstall=false;
$fxupdate=false;
$repeat=false;
if(($dberror === 'inst') || ($_POST['dbtinstall']))
{
	$fxinstall=true;
	$buttonvalue='dbtinstall';
	$err_iu_title=$GLOBALS['_itla']['ms_error'].': '.$GLOBALS['_itla']['ms_installation'];
}
else
{
	$fxupdate=true;
	$buttonvalue='dbtupdate';
	$err_iu_title=$GLOBALS['_itla']['ms_error'].': '.$GLOBALS['_itla']['ms_update'];
}
//fxDebug($fxinstall,'$fxinstall: $buttonvalue='.$buttonvalue, 0);
//fxDebug($fxupdate,'$fxupdate: $buttonvalue='.$buttonvalue, 0);

if($_POST['dbtinstall'])
	$dbtmode=1;
else if($_POST['dbtupdate'])
	$dbtmode=2;
else
	$dbtmode=0;
//fxDebug($dbtmode,'$dbtmode', 0);

// Scan DAT-subdirector for database definition files
$dberrorcode=0;
$swritelog='';
plog(-1, '', 0, __LINE__, __FILE__);
$sfa=fxScanDir($GLOBALS['datpath'], false, true, '/^.+\_vrs\.txt$/i');
//fxDebug($sfa,'$sfa', 0);
// ...No database definition files?
if(!sizeof($sfa['files']))
{
	leave(4,$err_iu_title,'<b>'.$GLOBALS['_itla']['err_iu_empty'].'</b><br /><br /><br /><hr class=fxhr><br /><br /><i class=grey>'.strtr($GLOBALS['_itla']['err_iu_note'], array('<!--#DATSUBDIR#-->'=>'&quot;<b>'.$GLOBALS['datpath'].'</b>&quot;', '<!--#FORMAT#-->'=>'&quot;<b>*_vrs.txt</b>&quot;', '<!--#FXDLLINK#-->'=>$download_link)).'</i>');
	$quit=true;
	return;
}
// ...No database defintion file for this version?
if(!isset($sfa['files'][$GLOBALS['fxpglobals']['dbparam']['versionscode_prg'].'_vrs.txt']))
{
	leave(4,$err_iu_title,'<b>'.strtr($GLOBALS['_itla']['err_iu_empty'], array('<!--#VERSION#-->'=>$GLOBALS['_itla']['ms_version'].$GLOBALS['fxpglobals']['dbparam']['version_prg'])).'</b><br /><br /><br /><hr class=fxhr><br /><br /><i class=grey>'.strtr($GLOBALS['_itla']['err_iu_note'], array('<!--#DATSUBDIR#-->'=>'&quot;<b>'.$GLOBALS['datpath'].'</b>&quot;', '<!--#FORMAT#-->'=>'&quot;<b>'.$GLOBALS['fxpglobals']['dbparam']['versionscode_prg'].'_vrs.txt</b>&quot;', '<!--#FXDLLINK#-->'=>$download_link)).'</i>');
	$quit=true;
	return;
}

$fq_cnt=0;
$fq=array();
$fdbtype=(int)$GLOBALS['fxpglobals']['dbparam']['dbtype'];

// Execute installation/update?
if($fxinstall || ($fxupdate && $GLOBALS['fxpglobals']['dbparam']['versionscode_prg'] > $GLOBALS['fxpglobals']['dbparam']['versionscode_db']))
{
	if(!$GLOBALS['fxpglobals']['client'])
		$GLOBALS['fxpglobals']['client']=1;

	// Installation
	if($fxinstall)
	{
		$buttontext=$GLOBALS['_itla']['ms_installation'].' '.$GLOBALS['_itla']['ms_version'].$GLOBALS['fxpglobals']['dbparam']['version_prg'];	// Installation V{New Version}
		if($dberror == 'inst2')
		{
			$repeat=true;
			$buttontext=$GLOBALS['_itla']['ms_repeat'].': '.$buttontext;	// Repeat: Installation V{New Version}
		}
	}

	// Update
	else
	{
		$buttontext=$GLOBALS['_itla']['ms_update'].' '.$GLOBALS['_itla']['ms_version'].$GLOBALS['fxpglobals']['dbparam']['version_db'].' &rarr; '.$GLOBALS['_itla']['ms_version'].$GLOBALS['fxpglobals']['dbparam']['version_prg'];	// Update V{Current Version} -> V{New Version}
		if($dberror == 'upd2')
		{
			$repeat=true;
			$buttontext=$GLOBALS['_itla']['ms_repeat'].': '.$buttontext;	// Repeat: Update V{Current Version} -> V{New Version}
		}
	}

	// Installation/Update
	$twidth=970;

	$content  = '<table width="'.$twidth.'" border="0" cellspacing="0" cellpadding="0">'.$nl;
	$content .= '<tr>'.$nl;
	$content .= '<td>'.$nl;
	$content .= '<div class="maskc"><div class="maskt">&nbsp;'.$buttontext.'</div><br />'.$nl;
	$content .= '<table width="100%" border="0" cellpadding="0" cellspacing="8">'.$nl;
	$content .= '<tr>'.$nl;

	// Notes before Installation/Update
	if(!$dbtmode)
	{
		$content .= '<td width="100%" height="100%" valign="top">'.$nl;
		$content .= '<table width="100%" height="100%" cellpadding="8" cellspacing="0">'.$nl;
		$content .= '<tr>'.$nl;
		$content .= '<td valign="top" align="center"><span style="position:relative;top:-40px;font-size:150px;margin-right:16px;color:#e1001a;text-shadow:2px 2px 3px rgba(0,0,0,0.50);">!</span></td>'.$nl;
		$content .= '<td valign="top">'.$nl;
		$content .= '<div style="margin-right:16px;">'.$nl;

		// Installation
		if($fxinstall)
			$content .= '<p align="justify">'.strtr($GLOBALS['_itla']['note_ins_time'], array('<!--#BUTTONTEXT#-->'=>'[<b>'.$buttontext.'</b>]')).'</p><br /><br /><p align="justify" class="blue"><u>'.strtoupper((string)$GLOBALS['_itla']['ms_important']).'!</u><br />'.$GLOBALS['_itla']['note_ins_once'].'</p>';
		// Update
		else
		{
			$content .= '<p align="justify">'.strtr($GLOBALS['_itla']['note_upd_time'], array('<!--#DBNAME#-->'=>'&quot;<b>'.$GLOBALS['fxpglobals']['dbparam']['dbname'].'</b>&quot;', '<!--#BUTTONTEXT#-->'=>'[<b>'.$buttontext.'</b>]')).'</p><br /><br /><p align="justify" class="blue"><u>'.strtoupper((string)$GLOBALS['_itla']['ms_important']).'!</u><br />'.$GLOBALS['_itla']['note_upd_once'].'</p>';
			if(($GLOBALS['fxpglobals']['dbparam']['versionscode_db'] < '021000000') && !function_exists('fxsppEcPasswd'))
				$content .= '<br /><br /><p align="justify" class="red"><u>'.strtoupper((string)$GLOBALS['_itla']['ms_note']).'!</u><br />'.strtr($GLOBALS['_itla']['note_upd_passwords'], array('<!--#FXINIADMPW#-->'=>$GLOBALS['fxpglobals']['dbparam']['adminpassword'])).'</p>';
		}

		$ps_met_act=(int)ini_get('max_execution_time');
		$ps_met_min=600;
		if(!$ps_met_act || ($ps_met_act >= $ps_met_min))
			$ps_met_col='green';
		else
			$ps_met_col='red';
		$ps_mit_act=(int)ini_get('max_input_time');
		$ps_mit_min=300;
		if($ps_mit_act >= $ps_mit_min)
			$ps_mit_col='green';
		else
			$ps_mit_col='red';
		$ps_ml_act_bytes=convBytes(ini_get('memory_limit'), false);	// -> Bytes
		$ps_ml_act=convBytes($ps_ml_act_bytes,'M');
		$ps_ml_min_bytes=268435456;
		$ps_ml_min=convBytes($ps_ml_min_bytes,'M');
		if($ps_ml_act_bytes >= $ps_ml_min_bytes)
			$ps_ml_col='green';
		else
			$ps_ml_col='red';
		$content .= '<br /><br /><br /><br /><hr class="fxfhr"><br /><u class="grey bold">php.ini</u><br /><table border="0"><tr><td nowrap valign="top"><i class="grey">&middot; max_execution_time=<font class="'.$ps_met_col.'">'.$ps_met_act.'</font>;</i>&nbsp;&nbsp;&nbsp;</td><td nowrap valign="top"><i class="grey">('.$GLOBALS['_itla']['ms_minimum_abr'].'='.$ps_met_min.')</i>&nbsp;&nbsp;&nbsp;</td><td><i class="grey">'.$GLOBALS['_itla']['php_met'].'</i></td></tr><tr><td nowrap valign="top"><i class="grey">&middot; max_input_time=<font class="'.$ps_mit_col.'">'.$ps_mit_act.'</font>;</i>&nbsp;&nbsp;&nbsp;</td><td nowrap valign="top"><i class="grey">('.$GLOBALS['_itla']['ms_minimum_abr'].'='.$ps_mit_min.')</i>&nbsp;&nbsp;&nbsp;</td><td><i class="grey">'.$GLOBALS['_itla']['php_mit'].'</i></td></tr><tr><td nowrap valign="top"><i class="grey">&middot; memory_limit=<font class="'.$ps_ml_col.'">'.$ps_ml_act.'</font>;</i>&nbsp;&nbsp;&nbsp;</td><td nowrap valign="top"><i class="grey">('.$GLOBALS['_itla']['ms_minimum_abr'].'='.$ps_ml_min.')</i>&nbsp;&nbsp;&nbsp;</td><td><i class="grey">'.$GLOBALS['_itla']['php_ml'].'</i></td></tr></table>'.$nl;

		$content .= '</div><br />'.$nl;
		$content .= '</td></tr></table>'.$nl;
		$content .= '</td></tr></table>'.$nl;
		$content .= '</div>'.$nl;
		$content .= '</td></tr></table>'.$nl;

		echo($content);

		$button=fieldSubmit($buttonvalue, $buttontext, 'sub', 0, false, false, true);

		// Language(s)
		$sla=fxGetAllLanguages(false,true);

		$quit=true;
		return;
	}

	if($fxinstall)
		$swritelog='ins_'.$GLOBALS['fxpglobals']['dbparam']['versionscode_prg'].'_'.$GLOBALS['datetime'];
	else
		$swritelog='upd_'.$GLOBALS['fxpglobals']['dbparam']['versionscode_db'].'-'.$GLOBALS['fxpglobals']['dbparam']['versionscode_prg'].'_'.$GLOBALS['datetime'];

	// Execute database actions
	if(!$dberrorcode)
	{
		if($fxinstall)
			db_slog('#Installation: V'.$GLOBALS['fxpglobals']['dbparam']['version_prg'], __LINE__, __FILE__);
		else
			db_slog('#Update: V'.$GLOBALS['fxpglobals']['dbparam']['version_db'].' -> V'.$GLOBALS['fxpglobals']['dbparam']['version_prg'], __LINE__, __FILE__);

		// Load definition text file
		$t=$GLOBALS['_itla']['dba_load_file'];
		db_slog('*'.$t, __LINE__, __FILE__);
		plog(0, $t.'...', 2, __LINE__, __FILE__);
		sleep(1);
		db_errorQueue(0, '<b class="darkgrey">'.$t.'</b>');
		$dbspath=$GLOBALS['datpath'].$GLOBALS['fxpglobals']['dbparam']['versionscode_prg'].'_vrs.txt';
		$fa=fxLoad($dbspath, 1);
		if(fxIsArray($fa))
		{
			// ...File exists
			db_errorQueue(0, '', $GLOBALS['_itla']['dba_file_exists'].': '.$dbspath);
			db_slog(strtoupper((string)$GLOBALS['_itla']['ms_ok']).': '.$GLOBALS['_itla']['dba_file_exists'].': '.$dbspath, __LINE__, __FILE__);

			// ...definition lines read
			db_errorQueue(1, '', sizeof($fa).' '.$GLOBALS['_itla']['dba_file_lines_read']);
			db_slog(sizeof($fa).' '.$GLOBALS['_itla']['dba_file_lines_read'], __LINE__, __FILE__);
		}
		else
		{
			$dberrorcode=2;
			// ...File missing
			db_errorQueue(-1, '', $GLOBALS['_itla']['dba_file_missing'].': '.$dbspath);
			db_slog(strtoupper((string)$GLOBALS['_itla']['ms_error']).'! '.$GLOBALS['_itla']['dba_file_missing'].': '.$dbspath, __LINE__, __FILE__);
		}

		$ps=2;

		// Update from commercial fx-project: Deleting all existing database views
		if(!$dberrorcode && $fxupdate && ($GLOBALS['fxpglobals']['dbparam']['versionscode_db'] < '021000000'))
		{
			$t=$GLOBALS['_itla']['dba_views_del'];
			db_slog('*'.$t, __LINE__, __FILE__);
			plog($ps, $t.'...', $ps+2, __LINE__, __FILE__);
			sleep(1);
			db_errorQueue(0, '<b class="darkgrey">'.$t.'</b>');
			dbViews(false);
			if(fxIsArray($GLOBALS['_dbva']))
			{
//fxDebug($GLOBALS['_dbva'],'$GLOBALS[\'_dbva\']', 0);
				// ...views found
				db_errorQueue(0, '', sizeof($GLOBALS['_dbva']).' '.$GLOBALS['_itla']['dba_views_found']);
				db_slog(sizeof($GLOBALS['_dbva']).' '.$GLOBALS['_itla']['dba_views_found'], __LINE__, __FILE__);

				$err=false;
				db_rollback('start');
				foreach($GLOBALS['_dbva'] as $vid => $va)
				{
					$sql="DROP VIEW ".$va['name'].$GLOBALS['fxpglobals']['dbparam']['dbcascade'];
//db_slog('NEXT SQL = ['.$sql.']', __LINE__, __FILE__);
					$success=dbDropView($sql);
					if(!$success)
					{
						$err=true;
						break;
					}
				}
				// ...Error
				if($err)
				{
					$dberrorcode=2;
					db_rollback('rollback');
					db_errorQueue(-1, '', $sql.$dberrmsg);
					db_slog(strtoupper((string)$GLOBALS['_itla']['ms_error']).'! '.$sql, __LINE__, __FILE__);
				}
				// ...Success
				else
				{
					db_rollback('ok');
					// All views successfully deleted
					db_errorQueue(1, '', $GLOBALS['_itla']['dba_views_all']);
					db_slog(strtoupper((string)$GLOBALS['_itla']['ms_ok']).': '.$GLOBALS['_itla']['dba_views_all'], __LINE__, __FILE__);
				}
				unset($GLOBALS['_dbva']);
			}
			else
			{
				// ...No views found
				db_errorQueue(1, '', $GLOBALS['_itla']['dba_views_no']);
				db_slog(strtoupper((string)$GLOBALS['_itla']['ms_ok']).': '.$GLOBALS['_itla']['dba_views_no'], __LINE__, __FILE__);
			}
			$ps += 2;
		}

		// Update from commercial fx-project: Deleting all outdated database tables
		if(!$dberrorcode && $fxupdate && ($GLOBALS['fxpglobals']['dbparam']['versionscode_db'] < '021000000'))
		{
			$t=$GLOBALS['_itla']['dba_tables_del'];
			db_slog('*'.$t, __LINE__, __FILE__);
			plog($ps, $t.'...', $ps+2, __LINE__, __FILE__);
			sleep(1);
			db_errorQueue(0, '<b class="darkgrey">'.$t.'</b>');
			dbStructure('*',false,false,false);
			if(fxIsArray($GLOBALS['_dbta']))
			{
//fxDebug($GLOBALS['_dbva'],'$GLOBALS[\'_dbva\']', 0);
				$dta=array();
				foreach($fa as $fl)
				{
					if(substr((string)$fl,0,3) == '*T|')
					{
						$pp=strpos($fl,'|',3);
						$ndbtn=strtolower((string)trim(substr((string)$fl,3,$pp-3)));
						$dta[$ndbtn]=$fl;
					}
				}
				if(sizeof($dta))
				{
					$ota=array();
					foreach($GLOBALS['_dbta'] as $odbtn => $odbta)
					{
						$odbtn=strtolower((string)trim((string)$odbtn));
						if(!isset($dta[$odbtn]))
						{
							$ota[$odbtn]=true;
//echo('<b class=red>'.$odbtn.'</b><br />');
						}
					}
					if(sizeof($ota))
					{
						// ...outdated table found
						db_errorQueue(0, '', sizeof($ota).' '.$GLOBALS['_itla']['dba_tables_found']);
						db_slog(sizeof($ota).' '.$GLOBALS['_itla']['dba_tables_found'], __LINE__, __FILE__);

						if($GLOBALS['_dbtype'] == 'sqlsrv')
							$dbo="dbo.";
						else
							$dbo="";

						$err=false;
						db_rollback('start');
						foreach($ota as $otn => $dummy)
						{

							$sql="DROP TABLE ".$dbo.$otn.$GLOBALS['fxpglobals']['dbparam']['dbcascade'];
//db_slog('NEXT SQL = ['.$sql.']', __LINE__, __FILE__);
							$success=db_query($sql);
							if(!$success)
							{
								$err=true;
								break;
							}
						}
						// ...Error
						if($err)
						{
							$dberrorcode=2;
							db_rollback('rollback');
							db_errorQueue(-1, '', $sql);
							db_slog(strtoupper((string)$GLOBALS['_itla']['ms_error']).'! '.$sql, __LINE__, __FILE__);
						}
						// ...Success
						else
						{
							db_rollback('ok');
							// All outdated tables successfully deleted
							db_errorQueue(1, '', $GLOBALS['_itla']['dba_tables_all']);
							db_slog(strtoupper((string)$GLOBALS['_itla']['ms_ok']).': '.$GLOBALS['_itla']['dba_tables_all'], __LINE__, __FILE__);
						}
					}
					else
					{
						// ...No outdated tables found
						db_errorQueue(1, '', $GLOBALS['_itla']['dba_tables_no']);
						db_slog(strtoupper((string)$GLOBALS['_itla']['ms_ok']).': '.$GLOBALS['_itla']['dba_tables_no'], __LINE__, __FILE__);
					}
				}
				else
				{
					$dberrorcode=2;
					// ...No definition file tables found
					db_errorQueue(-1, '', $GLOBALS['_itla']['dba_tables_defno']);
					db_slog(strtoupper((string)$GLOBALS['_itla']['ms_error']).'! '.$GLOBALS['_itla']['dba_tables_defno'], __LINE__, __FILE__);
				}
				unset($GLOBALS['_dbta']);
			}
			else
			{
				$dberrorcode=2;
				// ...No database tables found
				db_errorQueue(-1, '', $GLOBALS['_itla']['dba_tables_dbno']);
				db_slog(strtoupper((string)$GLOBALS['_itla']['ms_error']).'! '.$GLOBALS['_itla']['dba_tables_dbno'], __LINE__, __FILE__);
			}
			$ps += 2;
		}

		// Update from commercial fx-project: Deleting all existing database indexes
		if(!$dberrorcode && $fxupdate && ($GLOBALS['fxpglobals']['dbparam']['versionscode_db'] < '021000000'))
		{
			$t=$GLOBALS['_itla']['dba_indexes_del'];
			db_slog('*'.$t, __LINE__, __FILE__);
			plog($ps, $t.'...', $ps+2, __LINE__, __FILE__);
			sleep(1);
			db_errorQueue(0, '<b class="darkgrey">'.$t.'</b>');
			dbIndexes();
			if(fxIsArray($GLOBALS['_dbia']))
			{
//fxDebug($GLOBALS['_dbia'],'$GLOBALS[\'_dbia\']', 0);
				// ...indexes found
				db_errorQueue(0, '', sizeof($GLOBALS['_dbia']).' '.$GLOBALS['_itla']['dba_indexes_found']);
				db_slog(sizeof($GLOBALS['_dbia']).' '.$GLOBALS['_itla']['dba_indexes_found'], __LINE__, __FILE__);

				$err=false;
				db_rollback('start');
				foreach($GLOBALS['_dbia'] as $itname => $ita)
				{
					foreach($ita as $iname => $ia)
					{
						$sql=dbDropIndexSQL($itname,$iname);
//db_slog('NEXT SQL = ['.$sql.']', __LINE__, __FILE__);
						$success=db_query($sql);
						if(!$success)
						{
							$err=true;
							break;
						}
					}
					if($err)
						break;
				}
				// ...Error
				if($err)
				{
					$dberrorcode=2;
					db_rollback('rollback');
					db_errorQueue(-1, '', $sql.$dberrmsg);
					db_slog(strtoupper((string)$GLOBALS['_itla']['ms_error']).'! '.$sql, __LINE__, __FILE__);
				}
				// ...Success
				else
				{
					db_rollback('ok');
					// All indexes successfully deleted
					db_errorQueue(1, '', $GLOBALS['_itla']['dba_indexes_all']);
					db_slog(strtoupper((string)$GLOBALS['_itla']['ms_ok']).': '.$GLOBALS['_itla']['dba_indexes_all'], __LINE__, __FILE__);
				}
				unset($GLOBALS['_dbva']);
			}
			else
			{
				// ...No indexes found
				db_errorQueue(1, '', $GLOBALS['_itla']['dba_indexes_no']);
				db_slog(strtoupper((string)$GLOBALS['_itla']['ms_ok']).': '.$GLOBALS['_itla']['dba_indexes_no'], __LINE__, __FILE__);
			}
			$ps += 2;
		}

		// Load and execute definition text files
		if(!$dberrorcode)
		{
			// Table structure, indexes + data
			$dbsot=array('tab'=>$GLOBALS['_itla']['dbt_tab'], 'ind'=>$GLOBALS['_itla']['dbt_ind'], 'dat'=>$GLOBALS['_itla']['dbt_dat']);

			$tp=$GLOBALS['_itla']['dba_file_inv'];	// Investigating definition text file <!--#FILENAME#-->
			$te=$GLOBALS['_itla']['dba_file_exec'];	// Executing definition text file <!--#FILENAME#-->
			foreach($dbsot as $dbs => $dbstxt)
			{
				// Definition text file
				db_errorQueue(0, '<b class="darkgrey">'.$GLOBALS['_itla']['dba_file'].'</b> [<b>'.$dbs.'</b>] <i class="grey">('.$dbstxt.')</i>');
				db_slog('*'.$GLOBALS['_itla']['dba_file'].': '.$dbs, __LINE__, __FILE__);

				$pa=array();
				if($dbs == 'tab')
				{
					$pm=12;
					$pe=18;
					plog($ps, str_replace('<!--#FILENAME#-->','&quot;'.$dbs.'&quot;',$tp).'...', $pm, __LINE__, __FILE__);
					sleep(1);
					if($fxinstall)
						dbStructure('*',false,false,false);
					else
						dbStructure();
					$pa=dbGetTablePKDiff($GLOBALS['fxpglobals']['dbparam']['versionscode_prg'],$fa,0);
				}
				else if($dbs == 'ind')
				{
					$ps=18;
					$pm=24;
					$pe=28;
					plog($ps, str_replace('<!--#FILENAME#-->','&quot;'.$dbs.'&quot;',$tp).'...', $pm, __LINE__, __FILE__);
					sleep(1);
					if($fxinstall)
						dbStructure('*',true,true,false);
					$pa=dbGetIndexDiff($GLOBALS['fxpglobals']['dbparam']['versionscode_prg'],0,$fa);
				}
				else if($dbs == 'dat')
				{
					$ps=28;
					if($fxupdate)
						$pm=80;
					else
						$pm=50;
					$pe=95;
					plog($ps, str_replace('<!--#FILENAME#-->','&quot;'.$dbs.'&quot;',$tp).'...', $pm, __LINE__, __FILE__);
					sleep(1);
					if($fxupdate)
						dbStructure();
					$pa=dbGetDataDiff($GLOBALS['fxpglobals']['dbparam']['versionscode_prg'],0,$fa);
				}
//fxDebug($pa,'$pa', 0);

				if(fxIsArray($pa) && ($pa['filelines'] > 0))
				{
					$tm=str_replace('<!--#FILENAME#-->','&quot;'.$dbs.'&quot;',$te);
					$ps=$pm;
					$po=$pm;
					plog($ps, $tm.'...', $pe, __LINE__, __FILE__);
					sleep(1);

					$sec=0;
					if($pa['status'] >= 0)
					{
						foreach($pa['sqlarray'] as $sql)
						{
							if(substr((string)$sql,0,1) != '*')
								$sec++;
						}
					}

					if($pa['status'] < 0)
					{
						db_errorQueue(-1, '', $pa['message']);
						db_slog(strtoupper((string)$GLOBALS['_itla']['ms_error']).'! '.$pa['message'], __LINE__, __FILE__);
						$dberrorcode=2;
					}
					else if($sec)
					{
						db_errorQueue(0, '', $pa['message']);
						db_slog($pa['message'], __LINE__, __FILE__);

						$dbc=0;
						$err=false;
						db_rollback('start');

						$GLOBALS['_dberrmsg']='';
						foreach($pa['sqlarray'] as $sql)
						{
							$pp=min($pe, $ps+ceil((($pe-$ps)*$dbc)/$sec));
//echo('<pre style="width:99%;white-space:normal;border:1px solid #cccccc;margin:0;margin-bottom:2px;padding:4px;border-radius:3px;"><b class="darkgrey">'.($dbc+1).' = '.$pp.'%</b> '.fxHtmlEncode($sql).'</pre>'.$GLOBALS['nl']);
							if(substr((string)$sql,0,1) == '*')	// New Table
							{
								if($dbc)
								{
									db_rollback('ok');
									db_rollback('start');
								}
								$tm=str_replace('<!--#FILENAME#-->','&quot;'.$dbs.': '.substr((string)$sql,1).'&quot;',$te);
								plog($pp, $tm.'...', $pp, __LINE__, __FILE__);
							}
							else
							{
//db_slog('NEXT SQL = ['.$sql.']', __LINE__, __FILE__);
								$success=dbExecuteQuery($sql);
								if(!$success)
								{
									$err=true;
									break;
								}
								else
								{
									$dbc++;
									if($pp != $po)
									{
										$po=$pp;
										plog($pp, $tm.'...', $pp, __LINE__, __FILE__);
									}
								}
							}
						}
						// ...Error
						if($err)
						{
							$dberrorcode=2;
							$dberrmsg='';
							if(strlen((string)$GLOBALS['_dberrmsg']))
								$dberrmsg='<hr /><b class="red">'.$GLOBALS['_dberrmsg'].'</b>';
							db_rollback('rollback');
							db_errorQueue(-1, '', $sql.$dberrmsg);
							db_slog(strtoupper((string)$GLOBALS['_itla']['ms_error']).'! '.$sql, __LINE__, __FILE__);
						}
						// ...Success
						else
						{
							db_rollback('ok');
							// All necessary <!--#CHANGES#--> database changes successfully executed
							$msg=strtr($GLOBALS['_itla']['dba_all_changes_made'], array('<!--#CHANGES#-->'=>$dbc));
							db_errorQueue(1, '', $msg);
							db_slog(strtoupper((string)$GLOBALS['_itla']['ms_ok']).': '.$msg, __LINE__, __FILE__);
						}
					}
					else
					{
						// Already up-to-date - No changes necessary
						$msg=$GLOBALS['_itla']['dba_uptodate'];
						db_errorQueue(1, '', $msg);
						db_slog(strtoupper((string)$GLOBALS['_itla']['ms_ok']).': '.$msg, __LINE__, __FILE__);
					}
				}
				else
				{
					// Empty definition file
					$msg=$GLOBALS['_itla']['dba_file_empty'];
					db_errorQueue(-1, '', $msg);
					db_slog(strtoupper((string)$GLOBALS['_itla']['ms_error']).'! '.$msg, __LINE__, __FILE__);
					$dberrorcode=1;
				}

				if($dberrorcode)
					break;
			}
		}

		$ps=95;

		// Update from commercial fx-project: Adjusting all incompatible administrator passwords
		if(!$dberrorcode && $fxupdate && ($GLOBALS['fxpglobals']['dbparam']['versionscode_db'] < '021000000') && !function_exists('fxsppEcPasswd'))
		{
			$t=$GLOBALS['_itla']['dba_pw_adm_adjust'];
			db_slog('*'.$t, __LINE__, __FILE__);
			plog($ps, $t.'...', $ps+1, __LINE__, __FILE__);
			sleep(1);
			db_errorQueue(0, '<b class="darkgrey">'.$t.'</b>');
			// ...Get all administrators
			$sql="SELECT mandanten_id, benutzer_id FROM benutzer WHERE mandanten_id>0 AND (benutzer_id=1 OR b_admin=1)";
			$ula=db_values($sql,'*');
			if(fxIsArray($ula))
			{
				$err=false;
				db_rollback('start');
				foreach($ula as $ua)
				{
					// ...Encode password
					$upw=fxf_ecPasswd($GLOBALS['fxpglobals']['dbparam']['adminpassword'], '', $ua['mandanten_id'], $ua['benutzer_id']);
					$sql="UPDATE benutzer SET passwort='".$upw."' WHERE benutzer_id=".$ua['benutzer_id']." AND mandanten_id=".$ua['mandanten_id'];
					$err=db_x($sql);
					if($err)
						break;
				}
				// ...Error
				if($err)
				{
					$dberrorcode=2;
					db_rollback('rollback');
					db_errorQueue(-1, '', $sql);
					db_slog(strtoupper((string)$GLOBALS['_itla']['ms_error']).'! '.$sql, __LINE__, __FILE__);
				}
				// ...Success
				else
				{
					db_rollback('ok');
					// administrators found: Password set to <!--#ADMPW#-->
					$msg=sizeof($ula).' '.strtr($GLOBALS['_itla']['dba_pw_adm_set'], array('<!--#ADMPW#-->'=>'&quot;<b>'.$GLOBALS['fxpglobals']['dbparam']['adminpassword'].'</b>&quot;'));
					db_errorQueue(1, '', $msg);
					db_slog(strtoupper((string)$GLOBALS['_itla']['ms_ok']).': '.$msg, __LINE__, __FILE__);
				}
			}
			else
			{
				// ...No administrators found
				db_errorQueue(1, '', $GLOBALS['_itla']['dba_pw_adm_no']);
				db_slog(strtoupper((string)$GLOBALS['_itla']['ms_ok']).': '.$GLOBALS['_itla']['dba_pw_adm_no'], __LINE__, __FILE__);
			}
			$ps++;
		}

		// Update from commercial fx-project: Adjusting all incompatible user passwords
		if(!$dberrorcode && $fxupdate && ($GLOBALS['fxpglobals']['dbparam']['versionscode_db'] < '021000000') && !function_exists('fxsppEcPasswd'))
		{
			$t=$GLOBALS['_itla']['dba_pw_usr_adjust'];
			db_slog('*'.$t, __LINE__, __FILE__);
			plog($ps, $t.'...', $ps+2, __LINE__, __FILE__);
			sleep(1);
			db_errorQueue(0, '<b class="darkgrey">'.$t.'</b>');
			// ...Get all users
			$sql="SELECT mandanten_id, benutzer_id, name_benutzer FROM benutzer WHERE mandanten_id>0 AND benutzer_id>1 AND (b_admin IS NULL OR b_admin=0)";
			$ula=db_values($sql,'*');
			if(fxIsArray($ula))
			{
				$err=false;
				db_rollback('start');
				foreach($ula as $ua)
				{
					// ...Encode password
					$upw=fxf_ecPasswd(strtolower((string)trim((string)$ua['name_benutzer'])), '', $ua['mandanten_id'], $ua['benutzer_id']);
					$sql="UPDATE benutzer SET passwort='".$upw."' WHERE benutzer_id=".$ua['benutzer_id']." AND mandanten_id=".$ua['mandanten_id'];
					$err=db_x($sql);
					if($err)
						break;
				}
				// ...Error
				if($err)
				{
					$dberrorcode=2;
					db_rollback('rollback');
					db_errorQueue(-1, '', $sql);
					db_slog(strtoupper((string)$GLOBALS['_itla']['ms_error']).'! '.$sql, __LINE__, __FILE__);
				}
				// ...Success
				else
				{
					db_rollback('ok');
					// users found: users found: Password set to lower case user name
					$msg=sizeof($ula).' '.$GLOBALS['_itla']['dba_pw_usr_set'];
					db_errorQueue(1, '', $msg);
					db_slog(strtoupper((string)$GLOBALS['_itla']['ms_ok']).': '.$msg, __LINE__, __FILE__);
				}
			}
			else
			{
				// ...No users found
				db_errorQueue(1, '', $GLOBALS['_itla']['dba_pw_usr_no']);
				db_slog(strtoupper((string)$GLOBALS['_itla']['ms_ok']).': '.$GLOBALS['_itla']['dba_pw_usr_no'], __LINE__, __FILE__);
			}
			$ps += 2;
		}

		// Installations-/Updatenachbereitungen
		if(!$dberrorcode)
		{
			if($fxinstall)
			{
				if($GLOBALS['fxpglobals']['lang'] == 1)
					$t='Installationsnachbereitungen ausführen';
				else
					$t='Execute installation wrap-ups';
			}
			else if($GLOBALS['fxpglobals']['lang'] == 1)
				$t='Updatenachbereitungen ausführen';
			else
				$t='Execute update wrap-ups';
			plog($ps, $t.'...', 100, __LINE__, __FILE__);
			sleep(1);
			db_errorQueue(0, '<b class="darkgrey">'.$t.'</b>');

			// DMS-Pfad anpassen bei Erstinstallation
			if(!$dberrorcode && $fxinstall)
			{
				db_slog('*Adjust DMS path', __LINE__, __FILE__);
				if($GLOBALS['_os'] == 'windows')
					$dmsp='C:/DMS';
				else
					$dmsp='/opt/dms';
				$sql="UPDATE s_einstellungswerte SET einstellungswert='".$dmsp."' WHERE einstellungswerte_id=18 AND benutzer_id=1 AND mandanten_id=0";
				$success=db_query($sql);
			}

			// Update
			if(!$dberrorcode && $fxupdate)
			{
				// DB-Korrekturskript ausführen
				$from_dbt_update=true;
				$req_fn=fxf_fn_reqFilename('db_correct');
				if(strlen((string)$req_fn))
				{
					require($req_fn);
					if($correct_err)
					{
						$dberrorcode=1;
						if($GLOBALS['fxpglobals']['lang'] == 1)
							db_errorQueue(-1, '', 'Fehler bei der Datenkorrektur');
						else
							db_errorQueue(-1, '', 'Error during the data correction');
					}
				}
			}

			if(!$dberrorcode)
			{
				if($GLOBALS['fxpglobals']['lang'] == 1)
					db_errorQueue(1, '', 'Alle Nachbereitungen erfolgreich abgeschlossen');
				else
					db_errorQueue(1, '', 'All wrap-ups successfully finished');
			}
		}

		if(!$dberrorcode)
			$dberrorcode=-1;
	}
//$dberrorcode=1;

	$fcol=array('00af6b', '888800', 'e1001a');

	$content .= '<td width="100%" valign="top">'.$nl;
	$content .= '<table border="0" width="100%" cellpadding="8" cellspacing="0">'.$nl;
	$content .= '<tr>'.$nl;
	$content .= '<td valign="top" align="center">';
	if($dberrorcode > 0)
		$content .= '<span style="position:relative;top:-25px;font-size:100px;margin-right:16px;color:#e1001a;text-shadow:2px 2px 3px rgba(0,0,0,0.50);">KO</span>';
	else
		$content .= '<span style="position:relative;top:-25px;font-size:100px;margin-right:16px;color:#00af6b;text-shadow:2px 2px 3px rgba(0,0,0,0.50);">OK</span>';
	$content .= '</td>'.$nl;
	$content .= '<td width="100%" valign="top">'.$nl;
	$content .= '<div style="margin-right:16px;border:1px solid #aaaaaa;border-radius:4px;">'.$nl;
	$content .= '<table border="0" width="100%" cellpadding="0" cellspacing="0">'.$nl;

	// Error?
	if($dberrorcode > 0)
	{
		db_slog('*Ending procedure for error', __LINE__, __FILE__);
		if(($dberror == 'inst') || ($dberror == 'upd'))
		{
			$repeat=true;
			if($GLOBALS['fxpglobals']['lang'] == 1)
				$buttontext='Wiederhole '.$buttontext;
			else
				$buttontext='Repeat '.$buttontext;
		}

		$tbex=db_check('wertetab','t');
		if($tbex && $fxupdate)
		{
			if($fxupdate)
				$sql="UPDATE wertetab SET tabwert='".$GLOBALS['fxpglobals']['dbparam']['version_db']."', zeitstempel='".$GLOBALS['fxpglobals']['dbparam']['date_version']."' WHERE wertetabid=10 AND mandanten_id=0";
			else
				$sql="UPDATE wertetab SET tabwert='0', zeitstempel='".$GLOBALS['fxpglobals']['dbparam']['date_version']."' WHERE wertetabid=10 AND mandanten_id=0";
			@db_query($sql);
		}

		db_slog('Repeat -> FAILURE!', __LINE__, __FILE__);
		$swritelog='';
	}
	else if($dberrorcode < 0)
	{
		db_slog('*Ending procedure for success', __LINE__, __FILE__);

		$jetzt=fx_date('YmdHis',time());

		// Successful installation -> Insert version into table
		if($fxinstall)
		{
			$sql  = "INSERT INTO tabellen";
			$sql .= " (id_tabellen, tabname, tabelleart, gesperrt, gesperrt_von, hat_angelegt, hat_geaendert, isexp, historie, transid, aenderungs_id, zeitstempel) ";
			$sql .= "VALUES";
			$sql .= " (0, '".$GLOBALS['fxpglobals']['dbparam']['versionscode_prg']."', 0, 1, 'fx-project', 'fx-project', NULL, 0, 0, 0, 0, '".$jetzt."')";
			@db_query($sql);
		}

		// Set new version number
		@db_query("UPDATE wertetab SET zeitstempel='".$jetzt."' WHERE wertetabid IN (10,11,12) AND mandanten_id=0");
		$GLOBALS['fxpglobals']['dbparam']['versionscode_db']=$GLOBALS['fxpglobals']['dbparam']['versionscode_prg'];
		$GLOBALS['fxpglobals']['dbparam']['version_db']=$GLOBALS['fxpglobals']['dbparam']['version_prg'];
		$GLOBALS['fxpglobals']['dbparam']['date_version']=$jetzt;
		$GLOBALS['fxpglobals']['dbparam']['version_patch']=0;
		$GLOBALS['fxpglobals']['dbparam']['date_patch']=$jetzt;

		db_slog('All done -> SUCCESS.', __LINE__, __FILE__);
		if($fxinstall)
		{
			if($GLOBALS['fxpglobals']['lang'] == 1)
				$t='Installation erfolgreich abgeschlossen';
			else
				$t='Installation successfully completed';
		}
		else if($GLOBALS['fxpglobals']['lang'] == 1)
			$t='Update erfolgreich abgeschlossen';
		else
			$t='Update successfully completed';
		plog(100, $t.'.', 100, __LINE__, __FILE__);
		$swritelog='';

		$buttonvalue='dbtlogin';
		if($fxinstall)
			$buttontext=get_text(18,'kurz',0,2);	// Create Client 1 + Main Administrator
		else
			$buttontext=get_text(662,'20');			// Login
		$pfn=$GLOBALS['datpath'].$GLOBALS['fxpglobals']['dbparam']['versionscode_db'].'_pat.txt';
		if(file_exists($pfn))
		{
			$buttonvalue='dbtpatch';
			if($GLOBALS['fxpglobals']['lang'] == 1)
				$buttontext='Patch einspielen + '.$buttontext;
			else
				$buttontext='Integrate Patch + '.$buttontext;
		}

		// Delete all mask definition files
		deleteMaskFiles();

		sleep(2);
	}

	// Show log
	if(!fxIsArray($GLOBALS['dbeq']))
	{
		if($GLOBALS['fxpglobals']['lang'] == 1)
			db_errorQueue(-1, '', 'Log leer');
		else
			db_errorQueue(-1, '', 'Log empty');
	}
//fxDebug($GLOBALS['dbeq'], 'dbeq');
	$rv=0;
	$dbec=0;
	$dbca=array(-1=>'e1001a', '787878', '009f6b');
	$brs='border-right:1px solid #cccccc;';
	foreach($GLOBALS['dbeq'] as $dbeqid => $dbeqd)
	{
		$bts='';
		if($dbec)
			$bts='border-top:1px solid #dddddd;';

		$content .= '<tr>'.$nl;
		// Timestamp
		$timestamp=substr((string)$dbeqd['start'],8,2).':'.substr((string)$dbeqd['start'],10,2).':'.substr((string)$dbeqd['start'],12,2);
		if($dbeqd['end'] != $dbeqd['start'])
			$timestamp .= '-'.substr((string)$dbeqd['end'],8,2).':'.substr((string)$dbeqd['end'],10,2).':'.substr((string)$dbeqd['end'],12,2);
		$content .= '<td class="rv'.$rv.' lightgrey" nowrap valign="top" style="padding:3px;'.$brs.$bts.'">&nbsp;'.$timestamp.'&nbsp;</td>'.$nl;
		// Action
		$message='';
		if(strlen((string)$dbeqd['message']))
			$message='<div style="margin-left:16px;">'.$dbeqd['message'].'</div>';
		$content .= '<td class="rv'.$rv.'" width="100%" valign="top" style="padding:3px;'.$brs.$bts.'">'.$dbeqd['action'].'&nbsp;&hellip;'.$message.'</td>'.$nl;
		// Status
		if($dbeqd['status'] < 0)		// KO
			$status='<span style="font-size:24px;color:#'.$dbca[$dbeqd['status']].';text-shadow:0 0 1px rgb(0,0,0);">KO</span>';
		else if($dbeqd['status'] > 0)	// OK
			$status='<span style="font-size:24px;color:#'.$dbca[$dbeqd['status']].';text-shadow:0 0 1px rgb(0,0,0);">OK</span>';
		else
			$status='<span style="font-size:24px;color:#'.$dbca[$dbeqd['status']].';text-shadow:0 0 1px rgb(0,0,0);">-</span>';
		$content .= '<td class="rv'.$rv.'" nowrap valign="bottom" style="padding:3px;'.$bts.'">&nbsp;'.$status.'&nbsp;</td>'.$nl;
		$content .= '</tr>'.$nl;

		$dbec++;

		$rv++;
		if($rv>1)
			$rv=0;
	}

	$content .= '</table></div></td></tr></table></td>'.$nl;
	$content .= '</tr></table></div></td></tr></table>'.$nl;

	echo($content);

	$button=fieldSubmit($buttonvalue, $buttontext, 'sub', 0, false, false, true);

	// Success message
	if($dberrorcode < 0)
	{
		if($fxinstall)
		{
			if($GLOBALS['fxpglobals']['lang'] == 1)
				$GLOBALS['ema']=array('001','Installation erfolgreich','<b>Herzlichen Gl&uuml;ckwunsch!</b><br /><br />fx-project wurde erfolgreich installiert.');
			else
				$GLOBALS['ema']=array('001','Installation successful','<b>Congratulations!</b><br /><br />fx-project was successfully installed.');
		}
		else
		{
			if($GLOBALS['fxpglobals']['lang'] == 1)
				$GLOBALS['ema']=array('001','Update erfolgreich','<b>Herzlichen Gl&uuml;ckwunsch!</b><br /><br />fx-project wurde erfolgreich upgedatet.');
			else
				$GLOBALS['ema']=array('001','Update successful','<b>Congratulations!</b><br /><br />fx-project was successfully updated.');
		}
	}

	plog(-1, '', 0, __LINE__, __FILE__);

	$quit=true;
	return;
}

$quit=false;
return;
?>