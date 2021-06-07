<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : tools_prj.inc                                                //
// Version     : 21.1                                                         //
// Begin       : 2020-08-20                                                   //
// Last Change : 2020-10-29                                                   //
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
 * Collection of project functions
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

$GLOBALS['__loaded_'.basename(__FILE__)]=true;


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * Prüfen, ob Vorgangsnummer eines übergebenen Projekts eindeutig ist
 *
 * @param various $pid       - Mandatory parameter:                  ???PARAMETER???
 * @param boolean $deepcheck - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function check_vn($pid, $deepcheck=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mid=$GLOBALS['fxpglobals']['client'];
	$pid=(int)$pid;
	if(!$pid)
		return false;

	// Vorgangsnummer des übergebenen Projektes ermitteln
	$sql="SELECT vorgangsnummer FROM projekte WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
	$vn=db_value($sql);

	// Check 1: Aktuelles Projekt
	$sql="SELECT projekt_id FROM projekte WHERE projekt_id<>".$pid." AND vorgangsnummer LIKE '".$vn."' AND mandanten_id=".$mid;
	$cpid=db_value($sql);
	if($cpid)
		return true;

	// Alle untergeordneten Projekte und Aufgaben ermitteln
	if($deepcheck)
	{
		$inf=inferior_vn($pid, 0);
		if($inf != "-666")
		{
			$sql="SELECT vorgangsnummer, projekt_id FROM projekte WHERE projekt_id IN (".$inf.") AND projekt_id<>".$pid." AND mandanten_id=".$mid;
			$apids=db_values($sql, '*');
			if(is_array($apid))
			{
				foreach($apids as $apid)
				{
					// Check 2: Untergeordneter Vorgang
					$sql="SELECT projekt_id FROM projekte WHERE projekt_id<>".$apid['projekt_id']." AND vorgangsnummer LIKE '".$apid['vorgangsnummer']."' AND mandanten_id=".$mid;
					$cpid=db_value($sql);
					if($cpid)
						return true;
				}
			}
		}
	}

	return false;
}

/**
 * ???FUNCTION???
 *
 * @param various $prid - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function check_projekt_id($prid)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mid	= $GLOBALS['fxpglobals']['client'];
	$prid	= (int)$prid;
	if($prid > 0)
		$prid=(int)db_value("SELECT projekt_id FROM projekte WHERE projekt_id=".$prid." AND mandanten_id=".$mid);
	if($prid <= 0)
		return -1;

	if(!$GLOBALS['fxpglobals']['person'] || ($GLOBALS['fxpglobals']['user'] == 1) || $GLOBALS['fxpglobals']['persdat']['roles'][2])
		return $prid;

	$pers_arr=prights_pers($prid);
	if(!is_array($pers_arr) || !$pers_arr[$GLOBALS['fxpglobals']['person']])
		return 0;

	// Prüfen, ob Person Recht auf Aktion hat
	$akb=array(1=>2,4,8,16);
	if($pers_arr[$GLOBALS['fxpglobals']['person']] & $akb[$GLOBALS['aktion']])
		return $prid;

	// Kein Recht vorhanden
	return 0;
}

/**
 * ???FUNCTION???
 *
 * @param various $pid               - Mandatory parameter:                  ???PARAMETER???
 * @param string  $headline          - Optional parameter (default = ''):    ???PARAMETER???
 * @param boolean $with_call_process - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function tt_path($pid, $headline='', $with_call_process=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mid=$GLOBALS['fxpglobals']['client'];
	$pid=(int)$pid;
	if($pid < 1)
		return '';


	$elt=$pid;
	$erm=array();
	$ear=array();
	$eaa=array();
	if($with_call_process)
	{
		$sql="SELECT vorgangsnummer, name_projekt FROM projekte WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
		$tmp=db_values($sql);
		if(is_array($tmp))
		{
			$eaa[$tmp['vorgangsnummer']]=no_null($tmp['vorgangsnummer']).': '.$tmp['name_projekt'];
			$ear[$tmp['vorgangsnummer']]='<b>'.no_null($tmp['vorgangsnummer']).' <font class=blue>'.strtr($tmp['name_projekt'], $GLOBALS['caption_strtr']).'</font></b>';
			$erm[$elt]=true;
		}
	}
	while($elt)
	{
		$sql  = "SELECT e.vorgangsnummer, e.name_projekt, e.projekt_id FROM projekte p, projekte e";
		$sql .= " WHERE e.projekt_id=p.elter AND e.mandanten_id=p.mandanten_id AND e.projekt_id>0 AND p.projekt_id=".$elt." AND p.mandanten_id=".$mid;
		$tmp=db_values($sql);
		if(is_array($tmp))
		{
			$elt=(int)$tmp['projekt_id'];
			if($erm[$elt])
				break;

			$eaa[$tmp['vorgangsnummer']]=no_null($tmp['vorgangsnummer']).': '.$tmp['name_projekt'];
			$ear[$tmp['vorgangsnummer']]='<b>'.no_null($tmp['vorgangsnummer']).'</b> '.strtr($tmp['name_projekt'], $GLOBALS['caption_strtr']);
			$erm[$elt]=true;
		}
		else
			break;
	}
	if(sizeof($ear))
	{
		ksort($ear);
		$spc	= 0;
		$pth_js	= '';
		foreach($ear as $e)
		{
			if($pth_js)
			{
				$pth_js .= '<br />';
				if($spc)
					$pth_js .= '<img src='.$GLOBALS['gfxpath'].'dmy.png aling=top width='.($spc*16).' height=1>';
				$pth_js .= '<img src='.$GLOBALS['gfxpath'].'mt_l2.png aling=top width=16 height=16>';
				$spc++;
			}
			$pth_js .= $e;
		}
		$tt=$pth_js;

		$ttc=' style="cursor:help;" tooltip="';
		if($headline)
			$ttc .= '<table width=100%><tr><td width=100% bgcolor=#dddddd>&nbsp;<b class=s4>'.$headline.'</b>&nbsp;</td></tr></table><br />';
		$ttc .= '<table border=0 cellpadding=4 cellspacing=0><tr><td nowrap>'.$tt.'</td></tr></table>"';

		$rarr=array('stooltip'=>$tt, 'ctooltip'=>$ttc, 'arr'=>$eaa);
		return $rarr;
	}

	return '';
}

/**
 * ???FUNCTION???
 *
 * @param various $arr      - Mandatory parameter:                ???PARAMETER???
 * @param string  $headline - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function tt_array($arr, $headline='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!fxIsArray($arr) && !strlen($arr))
		return '';

	// JavaScript-Tooltip
	$tt=' style="cursor:help;" tooltip="';
	if($headline)
		$tt .= "<div style=background:#dddddd;>&nbsp;<b>".$headline."</b>&nbsp;</div><br />";
	$tt .= "<table border=0 cellpadding=0 cellspacing=1>";
	if(fxIsArray($arr))
	{
		$lc=0;
		foreach($arr as $title => $value)
		{
			$tt .= "<tr><td bgcolor=#eeeeee nowrap>&nbsp;".$title.":&nbsp;</td><td bgcolor=#ffffff>&nbsp;".$value."&nbsp;</td></tr>";
			$lc++;
		}
	}
	else
		$tt .= "<tr><td>&nbsp;".$arr."&nbsp;</td></tr>";
	$tt .= "</table>";
	$tt .= '"';

	return $tt;
}

/**
 * ???FUNCTION???
 *
 * @param string  $pidsql  - Optional parameter (default = ''):   ???PARAMETER???
 * @param string  $pers    - Optional parameter (default = ''):   ???PARAMETER???
 * @param boolean $addpers - Optional parameter (default = true): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function estimations($pidsql='', $pers='', $addpers=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mid=$GLOBALS['fxpglobals']['client'];
	$est=array();

	$vpid="";
	// Vorgänge ermitteln, welche eine zeitbedingte Schätzung benötigen
	$sqlz=" FROM projektfortschritt p, zeitdaten z";
	$sqlz .= " WHERE z.projekt_id=p.projekt_id AND z.mandanten_id=p.mandanten_id AND z.zeitart=102 AND p.aktiv_kz=1 AND p.zeitpunkt=1792";
	$sqlz .= " AND (p.personen_id IS NULL OR p.personen_id=0) AND p.mandanten_id=".$mid;
	if(strlen($pidsql))
		$sqlz .= " AND (".$pidsql.")";
	$sql="SELECT p.projekt_id, p.tage, z.soll_beginn".$sqlz;
	$tmp=db_values($sql, '*');
	if(is_array($tmp))
	{
		$vpid="SELECT p.projekt_id".$sqlz;

		$jetzt=substr($GLOBALS['datetime'],0,8).'000000';
		foreach($tmp as $t)
		{
			$sw=false;
			$sb=substr($t['soll_beginn'], 0, 8).'000000';
			$dd=fxp_date_diff($sb, $jetzt, 'D');
			$tg=(int)$t['tage'];
			if($dd < 0)
				$dtm=substr(time_add($sb, $tg, 'D'), 0, 8).'000000';
			else if($tg < 2)
			{
				$sw=true;
				$dtm=$jetzt;
			}
			else
			{
				// Anzahl der Zeitperioden berechnen, jedoch mind. 1
				$pp=max(1, (int)floor($dd/$tg));
				$dtm=substr(time_add($sb, $pp*$tg, 'D'), 0, 8).'000000';
				if($dtm < $jetzt)
					$sw=true;
			}
//echo('id: '.$t['projekt_id'].' - soll-beginn: '.$t['soll_beginn'].', jetzt: '.$jetzt.' - diff: '.$dd.', tage: '.$tg.' -- dtm: '.$dtm.'<hr />');
			$est[$t['projekt_id']]=array('soll_beginn'=>$t['soll_beginn'], 'tage'=>$tg, 'datum'=>$dtm, 'sollwert'=>$sw);
			if($addpers)
				$est[$t['projekt_id']]['pers']='';
		}
	}

	// Letztes Schätzungsdatum von Personen ermitteln
	if($addpers && strlen($vpid))
	{
		$sql="SELECT projekt_id, personen_id, zeitstempel FROM projektfortschritt WHERE aktiv_kz=1 AND projekt_id IN (".$vpid.") AND mandanten_id=".$mid;
		if(strlen($pers))
			$sql .= " AND personen_id IN (".$pers.")";
		else
			$sql .= " AND personen_id IS NOT NULL AND personen_id>0";
		$tmp=db_values($sql, '*');
		if(is_array($tmp))
		{
			foreach($tmp as $t)
				$est[$t['projekt_id']]['pers'][$t['personen_id']]=$t['zeitstempel'];
		}
	}
//fxDebug($est, '$est');

	return $est;
}

/**
 * ???FUNCTION???
 *
 * @param integer $budgen     - Optional parameter (default = 1): ???PARAMETER???
 * @param integer $show_debug - Optional parameter (default = 0): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function new_budget_summe($budgen=1, $show_debug=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$inserts=0;

	// Prüfen, ob die Einträge aus den alten Tabellen bereits in die neuen übertragen wurden
	$budget_done=false;
	$sql="SELECT projekt_id FROM budget_summe WHERE mandanten_id=0";
	$tmp=db_value($sql);
//fxDebug($tmp, $sql);
	if(strlen($tmp))
		$budget_done=true;

	// Alle Mandanten ermitteln
	$all_clients='';
	if(!$budget_done)
	{
		$sql="SELECT mandanten_id, aenderungs_id FROM projekte WHERE projektart=".FXP_PRJ_MAIN." AND mandanten_id>0 GROUP BY mandanten_id, aenderungs_id ORDER BY mandanten_id, aenderungs_id DESC";
		$tmp=db_values($sql, '*');
		if(is_array($tmp))
		{
			foreach($tmp as $t)
			{
				$tmid=(int)$t['mandanten_id'];
				$tpid=(int)$t['aenderungs_id'];
				if(!is_array($all_clients))
					$all_clients=array($tmid=>$tpid);
				else if(!isset($all_clients[$tmid]))
					$all_clients[$tmid]=$tpid;
			}
		}
//fxDebug($all_clients, $sql);
	}

	$ierr=false;
	// Alle Mandanten der Reihe nach durchgehen
	if(is_array($all_clients))
	{
		db_rollback('start');

		$GLOBALS['autoupdate']=true;
		$mid_backup	= $GLOBALS['fxpglobals']['client'];
		$prs_backup	= $GLOBALS['fxpglobals']['person'];

		foreach($all_clients as $cno => $cid)
		{
			$GLOBALS['fxpglobals']['client']=(int)$cno;
			$GLOBALS['fxpglobals']['person']=(int)$cid;

			// Alle Budgetwerte für Mandanten ermitteln
			$bud=get_budget_summe($cno, $budgen, $show_debug);

			// Einträge in Tabellen übertragen
			if(is_array($bud))
			{
				foreach($bud as $vn => $b)
				{
					$ierr=update_budget($b['projekt_id'], $b);
					if($ierr)
					{
						if($show_debug)
							fxDebug($b, 'update_budget: ['.$cno.'] - '.$vn);
						if(($GLOBALS['from_dbt_update'] === true) && ($ierr != 701))
						{
							db_slog('ERROR! Logic error in update IGNORED! update_budget: ['.$cno.'] - '.$vn.' (ID: '.$b['projekt_id'].') -> '.$ierr, __LINE__, __FILE__);
							$ierr=false;
							$inserts++;
						}
						else
						{
							db_slog('ERROR! update_budget: ['.$cno.'] - '.$vn.' (ID: '.$b['projekt_id'].') -> '.$ierr, __LINE__, __FILE__);
							break;
						}
					}
					else
						$inserts++;
				}
			}

			if($ierr)
				break;
		}

		// Alles OK, dann alle existierenden veralteten Warnmeldungs-SQL (2159) für neues Budget anpassen
		if(!$ierr)
		{
			$pchk=(int)db_value("SELECT projekt_id FROM warnungen WHERE warnung_id=2159 AND mandanten_id>0");
			if($pchk)
			{
				$psql="SELECT projekt_id FROM budget_summe WHERE (budget_pers_int_verbr + budget_mat_int_verbr) > ((budget_pers_int_gen + budget_mat_int_gen) * prozentsatz_sql) AND mandanten_id>0";
				$sql  = "UPDATE warnungen SET pruef_sql='".convert_string($psql, 'todb')."' WHERE warnung_id=2159 AND mandanten_id>0";
				$succ=db_query($sql);
				if(!$succ)
				{
					$ierr=true;
					db_slog('ERROR! $sql: "'.$sql.'"', __LINE__, __FILE__);
				}
			}
		}

		// Alles OK, dann Zeichen setzen dass Tabelle bereits geupdatet wurde
		if(!$ierr)
		{
			$sql  = "INSERT INTO budget_summe ";
			$sql .= "(mandanten_id, projekt_id, projektart, abrechnungsart, abrechnungsart_mat, budgettyp, ek_voraus, vk_voraus, ek, vk,";
			$sql .= " budgetrahmen_pers_int, budgetrahmen_pers_ext, budgetrahmen_mat_int, budgetrahmen_mat_ext,";
			$sql .= " budget_pers_int_gepl, budget_pers_ext_gepl, budget_mat_int_gepl, budget_mat_ext_gepl,";
			$sql .= " budget_pers_int_gen, budget_pers_ext_gen, budget_mat_int_gen, budget_mat_ext_gen,";
			$sql .= " budget_pers_int_akt, budget_pers_ext_akt, budget_mat_int_akt, budget_mat_ext_akt,";
			$sql .= " budget_pers_int_verpl, budget_pers_ext_verpl, budget_mat_int_verpl, budget_mat_ext_verpl,";
			$sql .= " budget_pers_int_verbr, budget_pers_ext_verbr, budget_mat_int_verbr, budget_mat_ext_verbr,";
			$sql .= " budget_pers_ext_abger, budget_mat_ext_abger,";
			$sql .= " rechnung_fest_ext, rechnung_std_ext, rechnung_reise_ext, rechnung_mat_ext,";
			$sql .= " rechnung_fest_ext_off, rechnung_std_ext_off, rechnung_reise_ext_off, rechnung_mat_ext_off) ";
			$sql .= "VALUES (0, 1969, 3, 2,2, 0,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0, 0,0,0,0, 0,0,0,0)";
			$ierr=db_x($sql);

			if($ierr)
				db_slog('ERROR! $sql: "'.$sql.'"', __LINE__, __FILE__);
		}

		if($show_debug)
			echo('INSERTED budgets: '.$inserts.'<hr />'.$nl);
		if($ierr)
		{
			if($show_debug)
				echo('<B>FEHLER:</B> Rollback!<br />'.$nl);
			db_rollback('rollback');
			db_slog('ERROR! Rollback.', __LINE__, __FILE__);
		}
		else
		{
			if($show_debug)
				echo('<B>OK!</B><br />'.$nl);
			db_rollback('ok');
		}

		$GLOBALS['fxpglobals']['client']=$mid_backup;
		$GLOBALS['fxpglobals']['person']=$prs_backup;
	}

	return $ierr;
}

/**
 * ???FUNCTION???
 *
 * @param integer $mid        - Optional parameter (default = 0):     ???PARAMETER???
 * @param boolean $budgen     - Optional parameter (default = false): ???PARAMETER???
 * @param integer $show_debug - Optional parameter (default = 0):     ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_budget_summe($mid=0, $budgen=false, $show_debug=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$cno=(int)$mid;
	if(!$cno)
		$cno=$GLOBALS['fxpglobals']['client'];

	if($budgen === false)
	{
		$budgen=0;
		if(strtoupper(substr($GLOBALS['fxpglobals']['settings']['budgetgen'], 0, 1)) == 'J')
			$budgen=1;
	}
	else
		$budgen=(int)$budgen;

	// Alte Budgetdaten des Mandanten ermitteln
	if($show_debug > 1)
		echo('Budgetdaten von Mandant [<B>'.$mid.'</B>] ermitteln...<br />');
	$sql  = "SELECT";
	$sql .= " projekt_id, vorgangsnummer, projektart";
	$sql .= " FROM projekte";
	$sql .= " WHERE mandanten_id=".$cno." AND projekt_id>0";
	$sql .= " ORDER BY vorgangsnummer";
	$prj=db_values($sql, '*');
//fxDebug($prj, $sql);

	// Neue Budgetstruktur erstellen
	$bud='';
	if(is_array($prj))
	{
		if($show_debug > 2)
			echo('<P CLASS="tb"><FONT CLASS="s2b">&nbsp;Projektdaten&nbsp;</FONT></P>'.$nl);

		$fpa='';
		foreach($prj as $p)
		{
			$vn=$p['vorgangsnummer'];

			// Leerer Budgeteintrag
			$bud[$vn]=array
			(
				'mandanten_id'				=> (int)$cno,
				'projekt_id'				=> (int)$p['projekt_id'],
				'projektart'				=> (int)$p['projektart'],
				'abrechnungsart'			=> FXP_INV_HOUR,	// Abrechnungsart (Personen)=Stundenbasis
				'abrechnungsart_mat'		=> FXP_INV_MAT_HU,	// Abrechnungsart (Material)=Mengenbasis
				'budgettyp'					=> 702,				// Standardbudget

				'budgetrahmen_pers_int'		=> 0.0,
				'budgetrahmen_pers_ext'		=> 0.0,
				'budgetrahmen_mat_int'		=> 0.0,
				'budgetrahmen_mat_ext'		=> 0.0,

				'budget_pers_int_gepl'		=> 0.0,
				'budget_pers_ext_gepl'		=> 0.0,
				'budget_mat_int_gepl'		=> 0.0,
				'budget_mat_ext_gepl'		=> 0.0,

				'budget_pers_int_gen'		=> 0.0,
				'budget_pers_ext_gen'		=> 0.0,
				'budget_mat_int_gen'		=> 0.0,
				'budget_mat_ext_gen'		=> 0.0
			);
		}

		// Interner Budgetrahmen und intern geplantes Budget
		$sql  = "SELECT";
		$sql .= " p.projekt_id, p.vorgangsnummer,";
		$sql .= " b.abrechnungsart, b.abrechnungsart_mat, b.budget_plan AS budgetrahmen_pers_int, b.budget AS budget_pers_int_gepl";
		$sql .= " FROM projekte p, budget b";
		$sql .= " WHERE b.projekt_id=p.projekt_id AND b.mandanten_id=".$cno." AND b.projekt_id>0";
		$sql .= " ORDER BY p.vorgangsnummer";
		$bin=db_values($sql, '*');
//fxDebug($bin, $sql);
		if(is_array($bin))
		{
			if($show_debug > 2)
				echo('<P CLASS="tb"><FONT CLASS="s2b">&nbsp;Internes Budget&nbsp;</FONT></P>'.$nl);
			foreach($bin as $p)
			{
				$vn=$p['vorgangsnummer'];
				if(!is_array($bud[$vn]))
					continue;

				// Abrechnungsart (Personen)
				$abr=(int)$b['abrechnungsart'];
				if(($abr == 2303) || ($abr == FXP_INV_MAT_HU))
					$abr=FXP_INV_HOUR;
				$bud[$vn]['abrechnungsart']=$abr;

				// Abrechnungsart (Material)
				$mabr=(int)$b['abrechnungsart_mat'];
				if(!$mabr || ($mabr == FXP_INV_HOUR))
					$mabr=FXP_INV_MAT_HU;
				$bud[$vn]['abrechnungsart_mat']=$mabr;

				// Interner Personenbudgetrahmen
				$b=(double)$p['budgetrahmen_pers_int'];
				if(($bud[$vn]['projektart'] <= FXP_PRJ_SUB) && ($b != 0.0))
				{
					if($show_debug > 2)
						echo('= <FONT CLASS="s1b" COLOR="#0000FF">Int. Personenbudgetrahmen</FONT> über "'.$b.' " setzen bei ['.$vn.'].<br />');
					$bud[$vn]['budgetrahmen_pers_int']=$b;
				}

				// Intern geplantes Budget bei Aufgaben
				$b=(double)$p['budget_pers_int_gepl'];
				if(($bud[$vn]['projektart'] == FXP_PRJ_TASK) && ($b != 0.0))
				{
					$tvn=$vn;
					$txt_b='';
					$txt_r='';
					while(strlen($tvn) > 3)
					{
						if(is_array($bud[$tvn]))
						{
							// Intern geplantes + genehmigtes Personenbudget
							if(!strlen($txt_b))
								$txt_b ='+ <FONT CLASS="s1b" COLOR="#DDAA00">Int. geplantes (+ genehmigtes) Personenbudget</FONT> über "'.$b.' " aufsummieren auf ['.$tvn.']';
							else
								$txt_b .= ' - ['.$tvn.']';
							$bud[$tvn]['budget_pers_int_gepl'] += $b;
							if($budgen)
								$bud[$tvn]['budget_pers_int_gen'] += $b;

							// Interner Personebudgetrahmen anpassen?
							if(($bud[$tvn]['budgetrahmen_pers_int'] != 0.0) && ($bud[$tvn]['budgetrahmen_pers_int'] < $bud[$tvn]['budget_pers_int_gepl']))
							{
								if(!strlen($txt_r))
									$txt_r ='= <FONT CLASS="s1b" COLOR="#0000FF">Int. Personenbudgetrahmen</FONT> über "'.$bud[$tvn]['budget_pers_int_gepl'].' " neu setzen bei ['.$tvn.']';
								else
									$txt_r .= ' - ['.$tvn.']';
								$bud[$tvn]['budgetrahmen_pers_int']=$bud[$tvn]['budget_pers_int_gepl'];
							}
						}
						$tvn=substr($tvn, 0, -5);
					}

					if($show_debug > 2)
					{
						if(strlen($txt_b))
							echo($txt_b.'.<br />');
						if(strlen($txt_r))
							echo($txt_r.'.<br />');
					}
				}
			}
		}

		// Extern geplantes Budget bei Aufgaben
		$sql  = "SELECT";
		$sql .= " p.projekt_id, p.vorgangsnummer, p.projektart, p.projekt_status,";
		$sql .= " b.betrag AS budget_pers_ext_gepl, b.datum AS anlage_datum, b.betrag_von_wem AS angelegt_von, b.betrag_grund AS textzeile";
		$sql .= " FROM projekte p, budget_betrag b";
		$sql .= " WHERE b.projekt_id=p.projekt_id AND b.mandanten_id=".$cno." AND p.projekt_id>0 AND p.projektart=".FXP_PRJ_TASK;
		$sql .= " ORDER BY p.vorgangsnummer";
		$btr=db_values($sql, '*');
//fxDebug($btr, $sql);
		if(is_array($btr))
		{
			if($show_debug > 2)
				echo('<P CLASS="tb"><FONT CLASS="s2b">&nbsp;Externes Budget&nbsp;</FONT></P>'.$nl);
			foreach($btr as $p)
			{
				$vn=$p['vorgangsnummer'];
				if(!is_array($bud[$vn]))
					continue;

				$b=(double)$p['budget_pers_ext_gepl'];

				$txt_b='';
				if($b != 0.0)
				{
					$tvn=$vn;
					while(strlen($tvn) > 3)
					{
						if(is_array($bud[$tvn]))
						{
							if(!strlen($txt_b))
								$txt_b ='+ <FONT CLASS="s1b" COLOR="#DDAA00">Ext. geplantes (+ genehmigtes) Personenbudget</FONT> über "'.$b.' " aufsummieren auf ['.$tvn.']';
							else
								$txt_b .= ' - ['.$tvn.']';
							$bud[$tvn]['budget_pers_ext_gepl'] += $b;
							if($budgen)
								$bud[$tvn]['budget_pers_ext_gen'] += $b;
						}

						$tvn=substr($tvn, 0, -5);
					}
				}

				if($show_debug > 2)
				{
					if(strlen($txt_b))
						echo($txt_b.'.<br />');
				}
			}
		}
	}

	return $bud;
}

/**
 * liefert äbhängige Projekte vom übergebenen Projekt
 *
 * @param various $pid - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_dependencies($pid)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pid=(int)$pid;
	if(!$pid)
		return false;

	$his_hir	= "";
	$old_dep	= array();
	$abh		= $pid;

	while(true)
	{
		// Steuervariable
		$check=false;
		// ermittelt alle Nachfolger vom Parameter $abh
		$sql="SELECT projekt_id, abhaengigkeit, vorgangsnummer FROM projekte WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND abhaengigkeit IN (".$abh.")";
		$dependent=db_values($sql, '*');
//fxDebug($dependent, $sql);
		if(is_array($dependent))
		{
			// Prüfen, ob die Abfrage eine bisher unbehandelte Abhängigkeit enthält
			foreach($dependent as $d)
			{
				$pid=$d['projekt_id'];

				// Wurde eine unbehandelte Abhängigkeit gefunden, dann $check=true ...
				if(!sizeof($old_dep) || !$old_dep[$pid])
				{
					$check=true;
					break;
				}
			}
		}

		// ... ansonsten wird abgebrochen
		if(!$check)
			break;

		// Es wurden zu behandelnde Abhängigkeiten gefunden
		foreach($dependent as $d)
		{
			// Speichern der bereits behandelten Abhängigkeiten
			$old_dep[$d['projekt_id']]=$d['abhaengigkeit'];

			$vn =$d['vorgangsnummer'];
			// untergeordnete Vorgänge des Vorgangs ermitteln
			if(strlen($his_hir))
				$his_hir .= " OR ";
			$his_hir="vorgangsnummer LIKE '".$vn."%'";

			// übergeordnete Vorgänge des Vorgangs ermitteln
			$evn=$vn;
			while(strlen($evn) > 5)
			{
				$evn=substr($evn, 0, -5);
				$his_hir .= " OR vorgangsnummer LIKE '".$evn."'";
			}
		}

		// erneute Abfrage mit den Hirarchien des abhängigen Vorgangs
		$abh="SELECT projekt_id FROM projekte WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND (".$his_hir.")";
	}

//fxDebug($old_dep, 'old_dep');
	if(sizeof($old_dep))
		return $old_dep;
	else
		return false;
}

/**
 * Addiert Aufwand-Soll oder Aufwand-Ist auf einen Vorgang und dessen Elter(n)
 *
 * @param various $Elter        - Mandatory parameter:               ???PARAMETER???
 * @param integer $aufwand_soll - Optional parameter (default = 0):  ???PARAMETER???
 * @param integer $aufwand_ist  - Optional parameter (default = 0):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function add_aufwand($Elter, $aufwand_soll=0, $aufwand_ist=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$Elter=trim($Elter);
	if(!strlen($Elter))
		return false;

	$aufwand_soll	= (int)$aufwand_soll;
	$aufwand_ist	= (int)$aufwand_ist;
	if(!$aufwand_soll && !$aufwand_ist)
		return false;

	// Eltern des Vorgangs ermitteln und übergebenen Elter anhängen
	$elters=superior_vn($Elter);

	// Aufwand-Soll oder -Ist
	$sql="UPDATE projekte SET";
	if($aufwand_soll)
		$sql .= " aufwand_soll=(aufwand_soll+(".$aufwand_soll."))";
	if($aufwand_ist)
	{
		if($aufwand_soll)
			$sql .= ",";
		$sql .= " aufwand_ist=(aufwand_ist+(".$aufwand_ist."))";
	}
	$sql .= " WHERE projekt_id IN (".$elters.")";
	$err=db_x($sql);

	return $err;
}

/**
 * ???FUNCTION???
 *
 * @param various $name            - Mandatory parameter:                ???PARAMETER???
 * @param string  $startdatum      - Optional parameter (default = ''):  ???PARAMETER???
 * @param string  $enddatum        - Optional parameter (default = ''):  ???PARAMETER???
 * @param string  $referenz        - Optional parameter (default = ''):  ???PARAMETER???
 * @param string  $beschreibung    - Optional parameter (default = ''):  ???PARAMETER???
 * @param string  $anmerkung       - Optional parameter (default = ''):  ???PARAMETER???
 * @param string  $anmerkung_int   - Optional parameter (default = ''):  ???PARAMETER???
 * @param string  $ansprechpartner - Optional parameter (default = ''):  ???PARAMETER???
 * @param string  $pid             - Optional parameter (default = ''):  ???PARAMETER???
 * @param string  $gakt            - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function project_action($name, $startdatum='', $enddatum='', $referenz='', $beschreibung='', $anmerkung='', $anmerkung_int='', $ansprechpartner='', $pid='', $gakt='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$perr=false;
	if(!$gakt)
		$gakt=$GLOBALS['aktion'];

	// Heutiges Datum
	$jetzt="'".$GLOBALS['datetime']."'";

	// Argumente
	if(!strlen($name))
		$name="'NEW'";

	// Referenz
	if(!strlen($referenz))
		$referenz='NULL';
	else if(substr($referenz, 0, 1) != "'")
		$referenz="'".convert_string($referenz, 'todb')."'";
	// Projektname
	if(substr($name, 0, 1) == "'")
		$pname=$name;
	else
		$pname="'".convert_string($name, 'todb')."'";
	// Beschreibung
	if(!strlen($beschreibung))
		$beschreibung='NULL';
	else if(substr($beschreibung, 0, 1) != "'")
		$beschreibung="'".convert_string($beschreibung, 'todb')."'";
	// Anmerkung
	if(!strlen($anmerkung))
		$anmerkung='NULL';
	else if(substr($anmerkung, 0, 1) != "'")
		$anmerkung="'".convert_string($anmerkung, 'todb')."'";
	// Anmerkung Intern
	if(!strlen($anmerkung_int))
		$anmerkung_int='NULL';
	else if(substr($anmerkung_int, 0, 1) != "'")
		$anmerkung_int="'".convert_string($anmerkung_int, 'todb')."'";
	// Ansprechpartner
	if(!strlen($ansprechpartner) || !(int)$ansprechpartner)
	{
		$ansprechpartner='NULL';
		$kunde='NULL';
	}
	else
	{
		$ansprechpartner=(int)$ansprechpartner;
		$kunde=(int)db_value("SELECT elter FROM personen WHERE personen_id=".$ansprechpartner." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
	}

	// Projekt- + Zeit-ID
	$pid=0;
	$zid=0;
	if(($gakt == FXP_CHANGE) || ($gakt == FXP_DELETE))
	{
		$pzb='';
		if($pid)
		{
			$sql="SELECT projekt_id, zeit_id FROM projekte WHERE projekt_id=$pid AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$pzb=db_values($sql);
		}
		else if($referenz != 'NULL')
		{
			$sql="SELECT projekt_id, zeit_id FROM projekte WHERE ref_nummer=$referenz AND projektart=59 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$pzb=db_values($sql);
		}
		if(!$pzb)
		{
			$sql="SELECT projekt_id, zeit_id FROM projekte WHERE name_projekt=$pname AND projektart=59 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$pzb=db_values($sql);
		}
		if(!$pzb)
			return false;
		else
		{
			$pid=$pzb['projekt_id'];
			$zid=$pzb['zeit_id'];
		}
	}

	// Start- und Enddatum
	if(!$startdatum)
	{
		if($zid)
			$startdatum="'".db_value("SELECT soll_beginn FROM zeitdaten WHERE zeit_id=".$zid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'])."'";
		else
			$startdatum=substr($jetzt,0,9)."000000'";
	}
	else if(substr($startdatum, 0, 1) != "'")
	{
		$derr=0;
		$startdatum="'".validate_entry($startdatum, $derr, 'datum')."'";
		if($derr)
			$perr=true;
	}

	if(!$enddatum)
	{
		if($zid)
			$enddatum="'".db_value("SELECT soll_ende FROM zeitdaten WHERE zeit_id=".$zid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'])."'";
		else
			$enddatum=substr($jetzt,0,9)."240000'";
	}
	else if(substr($enddatum, 0, 1) != "'")
	{
		$derr=0;
		$enddatum="'".validate_entry($enddatum, $derr, 'datum')."'";
		if($derr)
			$perr=true;
	}
	if($enddatum < $startdatum)
	{
		$zdatum=$enddatum;
		$enddatum=$startdatum;
		$startdatum=$zdatum;
	}

	// Abrechnungsart (Personen + Material)
	if($GLOBALS['fxpglobals']['settings']['def_abrechnart'])
	{
		$abrechnungsart=$GLOBALS['fxpglobals']['settings']['def_abrechnart'];
		if(!$abrechnungsart || ($abrechnungsart == FXP_INV_MAT_HU))
			$abrechnungsart=FXP_INV_HOUR;
		$abrechnungsart_mat=$abrechnungsart;
		if($abrechnungsart_mat == FXP_INV_HOUR)
			$abrechnungsart_mat=FXP_INV_MAT_HU;
	}
	else
	{
		$abrechnungsart=FXP_INV_HOUR;		// Stundenbasis
		$abrechnungsart_mat=FXP_INV_MAT_HU;	// Mengenbasis
	}

	if(!$perr)
	{
		switch($gakt)
		{
			case 2:	// Neuanlage
				// Neue ID´s holen
				$pid=id_max('projekte', 'projekt_id');
				$GLOBALS['newmpid']=$pid;
				$zid=id_max('zeitdaten', 'zeit_id');

				// Neue Vorgangsnummer errechnen
				$vsql="SELECT MAX(vorgangsnummer) AS mvn FROM projekte WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND projektart=".FXP_PRJ_MAIN." AND elter=0";
				$vval=(int)db_value($vsql);
				$vorgangsnummer=substr('0000'.MIN(9999, ($vval+1)), -4);

				// Projekte
				$pdaten=array(
					'mandanten_id'		=> $GLOBALS['fxpglobals']['client'],
					'projekt_id'		=> $pid,
					'mutterprojekt_id'	=> $pid,
					'zeit_id'			=> $zid,
					'budget_id'			=> 0,
					'elter'				=> 0,
					'vorgangsnummer'	=> "'$vorgangsnummer'",
					'name_projekt'		=> $pname,
					'projektart'		=> 59,	// Mutterprojekt
					'beschreibung'		=> $beschreibung,
					'kategorie_id'		=> 377,	// Neu
					'kunde'				=> $kunde,
					'ansprechpartner'	=> $ansprechpartner,
					'projektmanager'	=> 0,
					'projektleiter'		=> 0,
					'vertreter'			=> 0,
					'projekt_status'	=> 297,	// geplant
					'prioritaet'		=> 446,	// mittel
					'verlauf'			=> 'NULL',
					'aufwand_soll'		=> 0,
					'aufwand_ist'		=> 0,
					'deadline'			=> $enddatum,
					'max_zeit_aufw'		=> 0,
					'maart_id'			=> 0,
					'mafaehigkeit_id'	=> 0,
					'anmerkung'			=> $anmerkung,
					'anmerkungintern'	=> $anmerkung_int,
					'ref_nummer'		=> $referenz,
					'abhaengigkeit'		=> 0,
					'wiederholungart'	=> 'NULL',
					'anzahl'			=> 'NULL',
					'erstelldatum'		=> $jetzt
				);
				$cols='';
				$vals='';
				foreach($pdaten as $column => $value)
				{
					$cols .= ', '.$column;
					$vals .= ', '.$value;
				}
				$psql='INSERT INTO projekte ('.substr($cols, 2).') VALUES ('.substr($vals, 2).')';
//fxDebug($pdaten, 'Projektdaten');
//fxDebug($psql, '$psql');

				// Zeitdaten
				$zdaten=array(
					'mandanten_id'		=> $GLOBALS['fxpglobals']['client'],
					'zuordnungs_mid'	=> $GLOBALS['fxpglobals']['client'],
					'personen_id'		=> 'NULL',
					'zeit_id'			=> $zid,
					'projekt_id'		=> $pid,
					'mutterprojekt_id'	=> $pid,
					'soll_beginn'		=> $startdatum,
					'soll_ende'			=> $enddatum,
					'soll_verzoeg'		=> 0,
					'ist_verzoeg'		=> 0,
					'zeitart'			=> 102
				);
				$cols='';
				$vals='';
				foreach($zdaten as $column => $value)
				{
					$cols .= ', '.$column;
					$vals .= ', '.$value;
				}
				$zsql='INSERT INTO zeitdaten ('.substr($cols, 2).') VALUES ('.substr($vals, 2).')';
//fxDebug($zdaten, 'Zeitdaten');
//fxDebug($zsql, '$zsql');

				// SQL´s ausführen
				if(!$perr)
					$perr=db_x($psql, 0);	// Projekte
				if(!$perr)
					$perr=db_x($zsql, 0);	// Zeitdaten

				// Budget
				if(!$perr)
				{
					// Daten aufbereiten
					$barr=array
					(
						'budgettyp' => 702, 'abrechnungsart' => $abrechnungsart, 'abrechnungsart_mat' => $abrechnungsart_mat,
						'budgetrahmen_pers_int'	=> 0.0,	'budgetrahmen_pers_ext'	=> 0.0,	'budgetrahmen_mat_int'	=> 0.0,	'budgetrahmen_mat_ext'	=> 0.0,
						'budget_pers_int_gepl'	=> 0.0,	'budget_mat_int_gepl'	=> 0.0,	'budget_pers_ext_gepl'	=> 0.0,	'budget_mat_ext_gepl'	=> 0.0
					);
					$perr=update_budget($pid, $barr);
				}

				// PCA-Code
				if(!$perr)
				{
					$npca=pca_code($pid);
					if($npca == '?')
						$perr=817; // Fehler beim Verwalten des PCA-Codes!
				}
			break;

			case 3:	// Ändern
				// Projekte
				$pdaten=array(
					'name_projekt'		=> $pname,
					'beschreibung'		=> $beschreibung,
					'deadline'			=> $enddatum,
					'anmerkung'			=> $anmerkung,
					'anmerkungintern'	=> $anmerkung_int,
					'ref_nummer'		=> $referenz,
					'ansprechpartner'	=> $ansprechpartner,
					'kunde'				=> $kunde
				);
				$sets='';
				foreach($pdaten as $column => $value)
					$sets .= ', '.$column.'='.$value;
				$psql='UPDATE projekte SET '.substr($sets, 2).' WHERE projekt_id='.$pid.' AND mandanten_id='.$GLOBALS['fxpglobals']['client'];
//fxDebug($pdaten, 'Projektdaten');
//fxDebug($psql, '$psql');

				// Zeitdaten
				$zdaten=array(
					'soll_beginn'		=> $startdatum,
					'soll_ende'			=> $enddatum
				);
				$sets='';
				foreach($zdaten as $column => $value)
					$sets .= ', '.$column.'='.$value;
				$zsql='UPDATE zeitdaten SET '.substr($sets, 2).' WHERE zeit_id='.$zid.' AND mandanten_id='.$GLOBALS['fxpglobals']['client'];
//fxDebug($zdaten, 'Zeitdaten');
//fxDebug($zsql, '$zsql');

				// SQL´s ausführen
				if(!$perr)
					$perr=db_x($psql, 0);	// Projekte
				if(!$perr)
					$perr=db_x($zsql, 0);	// Zeitdaten
			break;

			case 4:	// Löschen
				// Prüfen, ob noch keine Unterprojekte existieren
				$up=db_value("SELECT projekt_id FROM projekte WHERE elter=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
				if(!$up)
				{
					// Projekte + Zeitdaten
					$psql="DELETE FROM projekte WHERE projekt_id=$pid AND mandanten_ID=".$GLOBALS['fxpglobals']['client'];
					$zsql="DELETE FROM zeitdaten WHERE zeit_id=$zid AND mandanten_ID=".$GLOBALS['fxpglobals']['client'];
//fxDebug($psql, '$psql');
//fxDebug($zsql, '$zsql');

					// SQL´s ausführen
					if(!$perr)
						$perr=db_x($psql, 0);	// Projekte
					if(!$perr)
						$perr=db_x($zsql, 0);	// Zeitdaten

					// Budget
					if(!$perr)
						$perr=update_budget($pid, false);

					// PCA-Code
					if(!$perr)
					{
						$npca=pca_code($pid);
						if($npca == '?')
							$perr=817; // Fehler beim Verwalten des PCA-Codes!
					}
				}
				else
					$perr=-99;
			break;
		}
	}

	return $perr;
}

/**
 * =============================================================================
 * PRIGHTS_GET
 * =============================================================================
 * Funktion liefert Projekt-SQL zurück mit den Projekt-ID´s auf denen die
 * Person Rechte besitzt.
 * -----------------------------------------------------------------------------
 * $prid			Projekt-ID (falls 0, dann werden alle zurückgegeben)
 * $aktionscode		falls angegeben Prüfung, ob Recht auf Aktion [Bitvergleich]
 * (2=Anzeige, 4=Neuanlage, 8=Ändern, 16=Löschen
 *
 * @param various $prid        - Mandatory parameter:                   ???PARAMETER???
 * @param integer $aktionscode - Optional parameter (default = 0):      ???PARAMETER???
 * @param boolean $inferior    - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function prights_get($prid, $aktionscode=0, $inferior=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	global $nl;

	// Variablen
	$pers	= $GLOBALS['fxpglobals']['person'];
	$ben	= $GLOBALS['fxpglobals']['user'];
	$mid	= $GLOBALS['fxpglobals']['client'];

	// Bestimmte Projektrechte
	$vn="";
	if($prid)
	{
		$vsql="SELECT vorgangsnummer FROM projekte WHERE projekt_id=".$prid." AND mandanten_id=".$mid;
		$vn=db_value($vsql);
		if(!strlen($vn))
			return "-666";
		else if($inferior)
			$vn .= "%";
	}

	$chk_id	= $prid.'_'.$aktionscode.'_'.$inferior;
	if(is_array($GLOBALS['_rem_prights']) && ($GLOBALS['_rem_prights']['id'] == $chk_id))
		return $GLOBALS['_rem_prights']['sql'];

	// Prüfen, ob Hauptadministrator oder Geschäftsführer
	if(!$pers || ($ben == 1) || $GLOBALS['fxpglobals']['persdat']['roles'][2])
	{
		$rsql="SELECT projekt_id FROM projekte WHERE projekt_id>0 AND mandanten_id=".$mid;
		if(strlen($vn))
			$rsql .= " AND vorgangsnummer LIKE '".$vn."'";

		$GLOBALS['_rem_prights']=array('id'=>$chk_id, 'sql'=>$rsql);
		return $rsql;
	}

	// Projektmanager, Projektleiter, Stellv. Projektleiter, AG-Ansprechpartner oder Änderung
	$rsql ="SELECT projekt_id FROM projekte WHERE mandanten_id=".$mid." AND (";
	$rsql .= "projektmanager=".$pers." OR projektleiter=".$pers." OR vertreter=".$pers." OR aenderungs_id=".$pers;
	if($aktionscode <= 2)
	{
		$rsql .= " OR ansprechpartner=".$pers." OR verlauf=".$pers;
		$rsql .= " OR elter IN (SELECT projekt_id FROM projekte WHERE (projektmanager=".$pers." OR projektleiter=".$pers." OR vertreter=".$pers.") AND mandanten_id=".$mid.")";
		$rsql .= " OR mutterprojekt_id IN (SELECT projekt_id FROM projekte WHERE (projektmanager=".$pers." OR projektleiter=".$pers." OR vertreter=".$pers.") AND mandanten_id=".$mid.")";
	}
	else
		$rsql .= " OR projektmanager=".$pers." OR projektleiter=".$pers." OR vertreter=".$pers;

	// Projektverantwortlich
	if($aktionscode <= 2)
		$rsql .= $nl." OR projekt_id IN (SELECT projekt_id FROM projekte_parameter WHERE parameter_txt='PV' AND parameter_int=".$pers." AND mandanten_id=".$mid.")";

	// Ressource
	if($aktionscode <= 2)
		$rsql .= $nl." OR projekt_id IN (SELECT projekt_id FROM maplanung WHERE personen_id=".$pers." AND mandanten_id=".$mid.")";

	// Rechtevergabe: Benutzer, Benutzergruppe + Abteilung
	$vlk='';
	$sbt='';
	if($aktionscode > 0)
		$sbt=" AND (r.aktionscode & ".$aktionscode.")>0";
	// ...Benutzer
	$sql="SELECT p.vorgangsnummer FROM rechte_referenz r, projekte p WHERE r.rechte_ref=37 AND r.gruppeart=2163 AND r.benutzergr_id=".$ben." AND r.mandanten_id=".$mid.$sbt." AND p.projekt_id=r.id_referenz AND p.mandanten_id=".$mid;
	$tmp=db_values($sql, '*');
	if(fxIsArray($tmp))
	{
		foreach($tmp as $t)
		{
			if(strlen($vlk))
				$vlk .= " OR ";
			$vlk .= "vorgangsnummer LIKE '".$t['vorgangsnummer']."%'";
		}
	}
	// ...Benutzergruppe
	if(!is_array($GLOBALS['_tmp_bgr']) || !isset($GLOBALS['_tmp_bgr'][$pers]))
	{
		$bgrar=get_benutzergruppen($ben);
		if(is_array($bgrar))
			$GLOBALS['_tmp_bgr'][$pers]=array_to_csv($bgrar);
		else
			$GLOBALS['_tmp_bgr'][$pers]='';
	}
	if(strlen($GLOBALS['_tmp_bgr'][$pers]))
	{
		$sql="SELECT p.vorgangsnummer FROM rechte_referenz r, projekte p WHERE r.rechte_ref=37 AND r.gruppeart=2164 AND r.benutzergr_id IN (".$GLOBALS['_tmp_bgr'][$pers].") AND r.mandanten_id=".$mid.$sbt." AND p.projekt_id=r.id_referenz AND p.mandanten_id=".$mid;
		$tmp=db_values($sql, '*');
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
			{
				if(strlen($vlk))
					$vlk .= " OR ";
				$vlk .= "vorgangsnummer LIKE '".$t['vorgangsnummer']."%'";
			}
		}
	}
	// ...Abteilung
	if(!is_array($GLOBALS['_tmp_abt']) || !isset($GLOBALS['_tmp_abt'][$pers]))
	{
		$abtar=get_abteilungen($pers);
		if(is_array($abtar))
			$GLOBALS['_tmp_abt'][$pers]=array_to_csv($abtar);
		else
			$GLOBALS['_tmp_abt'][$pers]='';
	}
	if(strlen($GLOBALS['_tmp_abt'][$pers]))
	{
		$sql="SELECT p.vorgangsnummer FROM rechte_referenz r, projekte p WHERE r.rechte_ref=37 AND r.gruppeart=2165 AND r.benutzergr_id IN (".$GLOBALS['_tmp_abt'][$pers].") AND r.mandanten_id=".$mid.$sbt." AND p.projekt_id=r.id_referenz AND p.mandanten_id=".$mid;
		$tmp=db_values($sql, '*');
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
			{
				if(strlen($vlk))
					$vlk .= " OR ";
				$vlk .= "vorgangsnummer LIKE '".$t['vorgangsnummer']."%'";
			}
		}
	}
	if(strlen($vlk))
		$rsql .= $nl." OR ".$vlk;

	$rsql .= ")";
	if(strlen($vn))
		$rsql .= $nl." AND vorgangsnummer LIKE '".$vn."'";

//echo('<b><u>prights_get</u> ('.$prid.', '.$aktionscode.', '.$inferior.')</b>:<pre>'.fxHtmlEncode($rsql).'</pre>'.$nl);
	$GLOBALS['_rem_prights']=array('id'=>$chk_id, 'sql'=>$rsql);
	return $rsql;
}

/**
 * =============================================================================
 * PRIGHTS_CHECK
 * =============================================================================
 * Funktion liefert true zurück, falls Person auf das Projekt Rechte besitzt,
 * sonst false.
 * -----------------------------------------------------------------------------
 * $prid			Projekt-ID (falls 0, dann wird überprüft ob Person
 * überhaupt auf irgendein Projekt Rechte besitzt)
 * $aktionscode		falls angegeben Prüfung, ob Recht auf Aktion [Bitvergleich]
 * (2=Anzeige, 4=Neuanlage, 8=Ändern, 16=Löschen
 *
 * @param various $prid        - Mandatory parameter:               ???PARAMETER???
 * @param integer $aktionscode - Optional parameter (default = 0):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function prights_check($prid, $aktionscode=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$idfound=(int)db_value(prights_get($prid, $aktionscode));
	if($idfound)
		return true;

	return false;
}

/**
 * =============================================================================
 * PRIGHTS_PERS
 * =============================================================================
 * Funktion liefert alle Personen-IDs als Array zurück, welche Rechte auf den
 * Vorgang besitzen, sonst false.
 * -----------------------------------------------------------------------------
 * $prid	Projekt-ID
 *
 * @param various $prid   - Mandatory parameter:                  ???PARAMETER???
 * @param boolean $retarr - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function prights_pers($prid, $retarr=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Bit 8=256	Hauptadmin / Geschäftsfüher
	// Bit 7=128	Hauptprojektverantwortliche (PM / PL / SPL)
	// Bit 6=64		Zusatzprojektverantwortliche / Auftraggeber
	// Bit 5=32		Ressourcen

	// Bit 4=16		Löschen
	// Bit 3=8		Ändern
	// Bit 2=4		Neuanlage
	// Bit 1=2		Anzeige

	// Bit 0=1		Projektersteller

	$pers_arr=array();
	$prid=(int)$prid;
	if($prid > 0)
	{
		$mid=$GLOBALS['fxpglobals']['client'];
		$ovn=db_value("SELECT vorgangsnummer FROM projekte WHERE projekt_id=".$prid." AND mandanten_id=".$mid);
		$vn=$ovn;
		if(strlen($vn))
		{
			$prid_sql="projekt_id IN (SELECT projekt_id FROM projekte WHERE vorgangsnummer='".$vn."'";
			while(strlen($vn) > 4)
			{
				$vn=substr($vn, 0, -5);
				$prid_sql .= " OR vorgangsnummer='".$vn."'";
			}
			$prid_sql .= ")";
		}
		else
			$prid_sql="projekt_id=".$prid;

		// Hauptadmin...
		$sql="SELECT personen_id FROM benutzer WHERE benutzer_id=1 AND mandanten_id=".$mid;
		$pers=(int)db_value($sql);
//fxDebug($pers, 'Hauptadmin: '.$sql);
		if($pers)
		{
			$code=256+30;
			$pers_arr[$pers]=$code;
		}

		// Geschäftsführer...
		$sql="SELECT personen_id FROM ma_art_faehigkeiten WHERE maart_id=1 AND mafaehigkeit_id=2 AND mandanten_id=".$mid;
		$tmp=db_values($sql, '*');
//fxDebug($tmp, 'Geschäftsführer: '.$sql);
		if(is_array($tmp))
		{
			$code=256+30;
			foreach($tmp as $t)
			{
				$pers=(int)$t['personen_id'];
				if($pers_arr[$pers])
					$pers_arr[$pers] |= $code;
				else
					$pers_arr[$pers]=$code;
			}
		}

		// Projektersteller...
		$sql="SELECT aenderungs_id FROM projekte_history WHERE projekt_id=".$prid." AND mandanten_id=".$mid." AND aktion=2";
		$pers=(int)db_value($sql);
//fxDebug($pers, 'Projektersteller: '.$sql);
		if($pers)
		{
			$code=1+30;
			$pers_arr[$pers]=$code;
		}

		// Projektmanager, Projektleiter, Stellv. Projektleiter, Ansprechpartner AG, ToDo-Ressource...
		$sql="SELECT projektmanager, projektleiter, vertreter, ansprechpartner, verlauf FROM projekte WHERE ".$prid_sql." AND mandanten_id=".$mid;
		$tmp=db_values($sql, '*');
		if(is_array($tmp))
		{
			$code_arr=array('projektmanager'=>128+30, 'projektleiter'=>128+30, 'vertreter'=>128+30, 'ansprechpartner'=>64+2, 'verlauf'=>32+2);
			if((strlen($ovn) == 4) && (strtolower(substr($GLOBALS['fxpglobals']['settings']['pm_aendert_mp'], 0, 1)) == 'j'))
			{
				$code_arr['projektleiter']	= 128+6;
				$code_arr['vertreter']		= 128+6;
			}

			foreach($tmp as $t)
			{
				foreach($code_arr as $prolle => $code)
				{
					$pers=(int)$t[$prolle];
					if(!$pers)
						continue;

					if($pers_arr[$pers])
						$pers_arr[$pers] |= $code;
					else
						$pers_arr[$pers]=$code;
				}
			}
		}

		// Zusatzprojektverantwortliche...
		$sql="SELECT parameter_int FROM projekte_parameter WHERE parameter_txt='PV' AND ".$prid_sql." AND mandanten_id=".$mid;
		$tmp=db_values($sql, '*');
//fxDebug($tmp, 'Zusatzprojektverantwortliche: '.$sql);
		if(is_array($tmp))
		{
			$code=64+2;
			foreach($tmp as $t)
			{
				$pers=(int)$t['parameter_int'];
				if($pers_arr[$pers])
					$pers_arr[$pers] |= $code;
				else
					$pers_arr[$pers]=$code;
			}
		}

		// Ressourcen...
		$sql="SELECT personen_id FROM maplanung WHERE personen_id>0 AND ".$prid_sql." AND mandanten_id=".$mid;
		$tmp=db_values($sql, '*');
//fxDebug($tmp, 'Ressourcen: '.$sql);
		if(is_array($tmp))
		{
			$code=32+2;
			foreach($tmp as $t)
			{
				$pers=(int)$t['personen_id'];
				if($pers_arr[$pers])
					$pers_arr[$pers] |= $code;
				else
					$pers_arr[$pers]=$code;
			}
		}

		// Benutzer...
		$sql  = "SELECT b.personen_id, r.aktionscode";
		$sql .= " FROM benutzer b, rechte_referenz r";
		$sql .= " WHERE r.rechte_ref=37 AND r.gruppeart=2163 AND r.id_referenz IN (SELECT projekt_id FROM projekte WHERE mandanten_id=".$mid." AND ".$prid_sql.") AND r.mandanten_id=".$mid;
		$sql .= " AND b.benutzer_id=r.benutzergr_id AND b.mandanten_id=".$mid;
		$tmp=db_values($sql, '*');
//fxDebug($tmp, 'Benutzer: '.$sql);
		if(is_array($tmp))
		{
			foreach($tmp as $t)
			{
				$pers=(int)$t['personen_id'];
				$code=(int)$t['aktionscode'];
				if((strlen($ovn) == 4) && ($code&8 || $code&16) && (strtolower(substr($GLOBALS['fxpglobals']['settings']['pm_aendert_mp'], 0, 1)) == 'j'))
					$code &= 65511;	// Bit 3+4 löschen
				if($pers_arr[$pers])
					$pers_arr[$pers] |= $code;
				else
					$pers_arr[$pers]=$code;
			}
		}

		// Benutzergruppen...
		$sql  = "SELECT b.personen_id, r.aktionscode";
		$sql .= " FROM benutzer b, benutzergruppenzo z, rechte_referenz r";
		$sql .= " WHERE r.rechte_ref=37 AND r.gruppeart=2164 AND r.id_referenz IN (SELECT projekt_id FROM projekte WHERE mandanten_id=".$mid." AND ".$prid_sql.") AND r.mandanten_id=".$mid;
		$sql .= " AND z.benutzergr_id=r.benutzergr_id AND z.mandanten_id=".$mid;
		$sql .= " AND b.benutzer_id=z.benutzer_id AND b.mandanten_id=".$mid;
		$tmp=db_values($sql, '*');
//fxDebug($tmp, 'Benutzergruppen: '.$sql);
		if(is_array($tmp))
		{
			foreach($tmp as $t)
			{
				$pers=(int)$t['personen_id'];
				$code=(int)$t['aktionscode'];
				if((strlen($ovn) == 4) && ($code&8 || $code&16) && (strtolower(substr($GLOBALS['fxpglobals']['settings']['pm_aendert_mp'], 0, 1)) == 'j'))
					$code &= 65511;	// Bit 3+4 löschen
				if($pers_arr[$pers])
					$pers_arr[$pers] |= $code;
				else
					$pers_arr[$pers]=$code;
			}
		}

		// Abteilungen...
		$sql  = "SELECT p.personen_id, r.aktionscode";
		$sql .= " FROM personen p, rechte_referenz r";
		$sql .= " WHERE r.rechte_ref=37 AND r.gruppeart=2165 AND r.id_referenz IN (SELECT projekt_id FROM projekte WHERE mandanten_id=".$mid." AND ".$prid_sql.") AND r.mandanten_id=".$mid;
		$sql .= " AND p.abt_id=r.benutzergr_id AND p.mandanten_id=".$mid;
		$tmp=db_values($sql, '*');
//fxDebug($tmp, 'Abteilungen: '.$sql);
		if(is_array($tmp))
		{
			foreach($tmp as $t)
			{
				$pers=(int)$t['personen_id'];
				$code=(int)$t['aktionscode'];
				if((strlen($ovn) == 4) && ($code&8 || $code&16) && (strtolower(substr($GLOBALS['fxpglobals']['settings']['pm_aendert_mp'], 0, 1)) == 'j'))
					$code &= 65511;	// Bit 3+4 löschen
				if($pers_arr[$pers])
					$pers_arr[$pers] |= $code;
				else
					$pers_arr[$pers]=$code;
			}
		}

		// Abteilungsleiter...
		$sql  = "SELECT a.abt_hperson, r.aktionscode";
		$sql .= " FROM abteilung a, rechte_referenz r";
		$sql .= " WHERE r.rechte_ref=37 AND r.gruppeart=2165 AND r.id_referenz IN (SELECT projekt_id FROM projekte WHERE mandanten_id=".$mid." AND ".$prid_sql.") AND r.mandanten_id=".$mid;
		$sql .= " AND a.abt_id=r.benutzergr_id AND a.abt_hperson IS NOT NULL AND a.abt_hperson>0 AND a.mandanten_id=".$mid;
		$tmp=db_values($sql, '*');
//fxDebug($tmp, 'Abteilungsleiter: '.$sql);
		if(is_array($tmp))
		{
			foreach($tmp as $t)
			{
				$pers=(int)$t['personen_id'];
				$code=(int)$t['aktionscode'];
				if((strlen($ovn) == 4) && ($code&8 || $code&16) && (strtolower(substr($GLOBALS['fxpglobals']['settings']['pm_aendert_mp'], 0, 1)) == 'j'))
					$code &= 65511;	// Bit 3+4 löschen
				if($pers_arr[$pers])
					$pers_arr[$pers] |= $code;
				else
					$pers_arr[$pers]=$code;
			}
		}
	}

	if(sizeof($pers_arr))
	{
//fxDebug($pers_arr, $prid);
		if($retarr)
			return $pers_arr;
		else
			return array_to_csv($pers_arr);
	}

	return false;
}

/**
 * =============================================================================
 * PRIGHTS_CODE
 * =============================================================================
 * Funktion liefert Aktionscode zurück, falls Person auf das Projekt Rechte
 * besitzt, sonst 0.
 * -----------------------------------------------------------------------------
 * $prid	Projekt-ID
 *
 * @param various $prid - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function prights_code($prid)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Bit 1=Anzeigen (2) - 2=Neuanlage (4) - 3=Ändern (8) - 4=Löschen (16)
	// Zusatzbits zeigen Rechtegruppen an (siehe prights_pers)

	$code=0;
	$prid=(int)$prid;
	if($prid > 0)
	{
		$pers=$GLOBALS['fxpglobals']['person'];
		$ben =$GLOBALS['fxpglobals']['user'];
		$mid =$GLOBALS['fxpglobals']['client'];

		// Prüfen, ob Hauptadministrator oder Geschäftsführer
		if(!$pers || ($ben == 1) || $GLOBALS['fxpglobals']['persdat']['roles'][2])
			$code=256+30;	// Recht auf alles
		else
		{
			$pers_arr=prights_pers($prid);
			if(is_array($pers_arr))
				$code=(int)$pers_arr[$pers];
		}
	}

	return $code;
}

/**
 * =============================================================================
 * PROJEKTINFO
 * =============================================================================
 * Funktion ermittelt alle wichtigen Informationen zu den Projekten und liefert
 * ein Array zurück.
 * -----------------------------------------------------------------------------
 * $pid			Projekt-ID; es werden alle untergeordneten Projekte sowie der
 * übergeordnete Pfad ermittelt und zurückgeliefert
 * $wholemp		falls true, wird gesamte Mutterprojekt-Struktur ermittelt
 *
 * @param various $pid      - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $wholemp  - Optional parameter (default = true):   ???PARAMETER???
 * @param boolean $todos    - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $fastinfo - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function projektinfo($pid, $wholemp=true, $todos=false, $fastinfo=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!$pid || ($pid < 0))
		return false;

	// Vorgangsnummer ermitteln
	$mid=$GLOBALS['fxpglobals']['client'];
	$sql="SELECT vorgangsnummer FROM projekte WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
	$vn =trim(db_value($sql));
	if(!strlen($vn))
		return false;

	// "Freigabe" (= approval)
	$app=prjTableHasApproval();

	if($wholemp)
		$vn=substr($vn, 0, 4);
	$vn_pri=substr($vn, 0, -5);

	// Vorgangsnummer-SQL-Bedingung
	$vn_sql='';
	while(strlen($vn_pri) > 3)
	{
		$vn_sql .= " OR p.vorgangsnummer='".$vn_pri."'";
		$vn_pri=substr($vn_pri, 0, -5);
	}
	$vn_sql .= " OR p.vorgangsnummer='".$vn."'";
	if(!$fastinfo)
		$vn_sql .= " OR p.vorgangsnummer LIKE '".$vn.".%'";

	// Projektinfos einlesen
	$sqlz =" FROM";
	$sqlz .= " projekte p, budget_summe b, zeitdaten z";
	$sqlz .= " WHERE";
	$sqlz .= " b.projekt_id=p.projekt_id AND z.projekt_id=p.projekt_id AND z.zeitart=102 AND p.mandanten_id=".$mid." AND (".substr($vn_sql, 4).")";
	$sqlz .= " AND p.projektart<=".FXP_PRJ_TASK." AND b.mandanten_id=".$mid." AND z.mandanten_id=".$mid;

	$sql  = "SELECT";
	$sql .= " p.projekt_id, p.elter, p.mutterprojekt_id, p.zeit_id,";
	$sql .= " NULL AS pca, p.vorgangsnummer, p.name_projekt, p.projektart, p.projekt_status, p.prioritaet,";
	$sql .= " p.kunde, p.ansprechpartner, p.projektmanager, p.projektleiter, p.vertreter,";
	$sql .= " p.aufwand_soll, p.aufwand_ist, p.aufwand_todo, p.max_zeit_aufw, p.deadline,";
	$sql .= " p.kategorie_id, p.maart_id, p.mafaehigkeit_id, p.abhaengigkeit,";
	$sql .= " p.beschreibung, p.anmerkung, p.anmerkungintern, p.ref_nummer, p.port,";
	if($app)
		$sql .= " p.freigabe,";
	$sql .= " p.verlauf, p.wiederholungart, p.anzahl, p.aenderungs_id AS angelegt_von, p.erstelldatum, p.aenderungs_id, p.zeitstempel,";

	$sql .= " b.ek_voraus, b.vk_voraus, b.ek, b.vk, b.abrechnungsart, b.abrechnungsart_mat, b.budgettyp,";

	$sql .= " b.budgetrahmen_pers_int, b.budgetrahmen_pers_ext, b.budgetrahmen_mat_int, b.budgetrahmen_mat_ext,";

	$sql .= " b.budget_pers_int_gepl, b.budget_pers_ext_gepl, b.budget_mat_int_gepl, b.budget_mat_ext_gepl,";
	$sql .= " b.budget_pers_int_gen, b.budget_pers_ext_gen, b.budget_mat_int_gen, b.budget_mat_ext_gen,";
	$sql .= " b.budget_pers_int_akt, b.budget_pers_ext_akt, b.budget_mat_int_akt, b.budget_mat_ext_akt,";
	$sql .= " b.budget_pers_int_verpl, b.budget_pers_ext_verpl, b.budget_mat_int_verpl, b.budget_mat_ext_verpl,";
	$sql .= " b.budget_pers_int_verbr, b.budget_pers_ext_verbr, b.budget_mat_int_verbr, b.budget_mat_ext_verbr,";
	$sql .= " b.budget_pers_ext_abger, b.budget_mat_ext_abger,";

	$sql .= " (b.budget_pers_int_gepl + b.budget_mat_int_gepl) AS budget_ges_int_gepl, (b.budget_pers_ext_gepl + b.budget_mat_ext_gepl) AS budget_ges_ext_gepl,";
	$sql .= " (b.budget_pers_int_gen + b.budget_mat_int_gen) AS budget_ges_int_gen, (b.budget_pers_ext_gen + b.budget_mat_ext_gen) AS budget_ges_ext_gen,";
	$sql .= " (b.budget_pers_int_akt + b.budget_mat_int_akt) AS budget_ges_int_akt, (b.budget_pers_ext_akt + b.budget_mat_ext_akt) AS budget_ges_ext_akt,";
	$sql .= " (b.budget_pers_int_verpl + b.budget_mat_int_verpl) AS budget_ges_int_verpl, (b.budget_pers_ext_verpl + b.budget_mat_ext_verpl) AS budget_ges_ext_verpl,";
	$sql .= " (b.budget_pers_int_verbr + b.budget_mat_int_verbr) AS budget_ges_int_verbr, (b.budget_pers_ext_verbr + b.budget_mat_ext_verbr) AS budget_ges_ext_verbr,";
	$sql .= " (b.budget_pers_ext_abger + b.budget_mat_ext_abger) AS budget_ges_ext_abger,";

	$sql .= " b.rechnung_fest_ext, b.rechnung_std_ext, b.rechnung_reise_ext, b.rechnung_mat_ext,";
	$sql .= " b.rechnung_fest_ext_off, b.rechnung_std_ext_off, b.rechnung_reise_ext_off, b.rechnung_mat_ext_off,";

	$sql .= " z.soll_beginn, z.soll_ende, z.gen_beginn, z.gen_ende, z.ist_beginn, z.ist_ende, z.soll_verzoeg, z.ist_verzoeg,";

	$sql .= " 0 AS pfid, 0 AS rfgrad, 1784 AS methode, 1790 AS zeitpunkt, NULL AS tage";

	$sql .= $sqlz;

	$sql .= " ORDER BY";
	$sql .= " p.vorgangsnummer";

	$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql);

	$pids="";
	$pinf=array();
	if(is_array($tmp))
	{
		$pids="SELECT p.projekt_id".$sqlz;

		foreach($tmp as $t)
		{
			$pid=(int)$t['projekt_id'];
			$pinf[$pid]=array
			(
				'aufwand_soll'=>(double)$t['aufwand_soll'], 'soll_verzoeg'=>(double)$t['soll_verzoeg'],
				'aufwand_ist'=>(double)$t['aufwand_ist'], 'ist_verzoeg'=>(double)$t['ist_verzoeg'],
				'sigrad'=>0.0, 'angrad'=>0.0, 'rfgrad'=>0.0, 'afgrad'=>0.0,
				'apaket'=>0, 'apaket_fertig'=>0,
				'rechte'=>2,
				'data'=>$t
			);

			if((int)$pinf[$pid]['data']['mafaehigkeit_id'])
				$pinf[$pid]['data']['fae']=(int)$pinf[$pid]['data']['mafaehigkeit_id'].'f';
			else
				$pinf[$pid]['data']['fae']=(int)$pinf[$pid]['data']['maart_id'];
		}
	}

	// Bestehende PCA-Codes ermitteln
	if(strlen($pids))
	{
		$sql  = "SELECT projekt_id, parameter_txt";
		$sql .= " FROM projekte_parameter";
		$sql .= " WHERE parameter_typ=376 AND parameter_var='TXT' AND projekt_id IN (".$pids.") AND mandanten_id=".$mid;
		$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql);
		if(is_array($tmp))
		{
			foreach($tmp as $t)
			{
				$apid=(int)$t['projekt_id'];
				$pinf[$apid]['data']['pca']=trim($t['parameter_txt']);
			}
		}
	}

	// Neue PCA-Codes generieren
	if(sizeof($pinf))
	{
		foreach($pinf as $pid => $p)
		{
			if(!strlen($pinf[$pid]['data']['pca']))
				$pinf[$pid]['data']['pca']=pca_code($pid);
		}
	}

	// Angelegt von ermitteln
	if(strlen($pids))
	{
		$sql  = "SELECT projekt_id, aenderungs_id";
		$sql .= " FROM projekte_history";
		$sql .= " WHERE aktion=2 AND aenderungs_id>0 AND projekt_id IN (".$pids.") AND mandanten_id=".$mid;
		$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql);
		if(is_array($tmp))
		{
			foreach($tmp as $t)
			{
				$apid=(int)$t['projekt_id'];
				$pinf[$apid]['data']['angelegt_von']=(int)$t['aenderungs_id'];
			}
		}
	}

	// Korrekten Budgettyp ermitteln oder Standardbudgettyp setzen
	if(strlen($pids))
	{
		$sql  = "SELECT projekt_id, budgettyp";
		$sql .= " FROM budget_detail";
		$sql .= " WHERE budgettyp IS NOT NULL AND budgettyp>0 AND projekt_id IN (".$pids.") AND mandanten_id=".$mid;
		$sql .= " ORDER BY zeitstempel DESC";
		$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql);
		if(is_array($tmp))
		{
			foreach($tmp as $t)
			{
				$apid=(int)$t['projekt_id'];

				if(!$pinf[$apid]['data']['budgettyp'])
					$pinf[$apid]['data']['budgettyp']=$t['budgettyp'];
			}
		}

		foreach($pinf as $pid => $p)
		{
			if(!$pinf[$pid]['data']['budgettyp'])
				$pinf[$pid]['data']['budgettyp']=702;	// Standardbudget
		}
	}

	// Projektfortschritt
	if(sizeof($pinf))
	{
		foreach($pinf as $pid => $p)
		{
			// Soll-Ist-Vergleich
			if($p['data']['projekt_status'] == FXP_PS_COMPLETED)
				$pinf[$pid]['sigrad']=1.0000;
			else if($p['data']['aufwand_soll'] == 0.0)
				$pinf[$pid]['sigrad']=0.000;
			else
				$pinf[$pid]['sigrad']=MIN(0.9999, $p['data']['aufwand_ist']/$p['data']['aufwand_soll']);

			// Anarbeitsungsgrad
			if($p['data']['projekt_status'] == FXP_PS_COMPLETED)
				$pinf[$pid]['angrad']=1.0000;
			else if ($p['data']['budget_ges_int_akt'] > 0.0)
				$pinf[$pid]['angrad']=MIN(0.9999, ($p['data']['budget_ges_int_verpl']+$p['data']['budget_ges_int_verbr']) / $p['data']['budget_ges_int_akt']);
			else if(($p['data']['budget_ges_int_gep'] == 0.0) || ($p['data']['budget_ges_int_gen'] == 0.0))
				$pinf[$pid]['angrad']=0.000;

			// Absoluten Fertigstellungsgrad
			if($p['data']['projektart'] == FXP_PRJ_TASK)
			{
				// Abs. Fertigstellungsgrad
				$pinf[$pid]['apaket']=1;
				if($p['data']['projekt_status'] == FXP_PS_COMPLETED)
					$pinf[$pid]['apaket_fertig']=1;
			}
			else
			{
				$sql="SELECT COUNT(projekt_id) AS apaket FROM projekte WHERE mandanten_id=".$mid." AND projektart=".FXP_PRJ_TASK." AND vorgangsnummer LIKE '".$p['data']['vorgangsnummer'].".%'";
				$pinf[$pid]['apaket']=(int)db_value($sql);

				$sql="SELECT COUNT(projekt_id) AS apaket_fertig FROM projekte WHERE mandanten_id=".$mid." AND projektart=".FXP_PRJ_TASK." AND projekt_status=".FXP_PS_COMPLETED." AND vorgangsnummer LIKE '".$p['data']['vorgangsnummer'].".%'";
				$pinf[$pid]['apaket_fertig']=(int)db_value($sql);
			}
			if($p['data']['projekt_status'] == FXP_PS_COMPLETED)
				$pinf[$pid]['afgrad']=1.00;
			else if($pinf[$pid]['apaket'] > 0)
				$pinf[$pid]['afgrad']=MIN(0.9999, $pinf[$pid]['apaket_fertig']/$pinf[$pid]['apaket']);

			// Relativer Fertigstellungsgrad
			$sql  = "SELECT id, pk_fgrad, methode, zeitpunkt, tage";
			$sql .= " FROM projektfortschritt";
			$sql .= " WHERE projekt_id=".$pid." AND (personen_id IS NULL OR personen_id=0) AND aktiv_kz=1 AND mandanten_id=".$mid;
			$tmp=db_values($sql);
//fxDebug($tmp, $pid);
			if(is_array($tmp))
			{
				// Aktuelle Informationen
				$vn=$pinf[$pid]['data']['vorgangsnummer'];
				$pinf[$pid]['data']['pfid']			= (int)$tmp['id'];
				$pinf[$pid]['data']['methode']		= (int)$tmp['methode'];
				$pinf[$pid]['data']['zeitpunkt']	= (int)$tmp['zeitpunkt'];
				$pinf[$pid]['data']['tage']			= (int)$tmp['tage'];
//echo('<b>Methode von ['.$vn.']='.$pinf[$pid]['data']['methode'].'</b><br />');

				// Rel. Fertigstellungsmethode von Projekten nach unten weitergeben als Default-Einstellung
				if($pinf[$pid]['data']['projektart'] != FXP_PRJ_TASK)
				{
					$ml=strlen($vn);
					foreach($pinf as $tpid => $tp)
					{
						$tvn=$tp['data']['vorgangsnummer'];
						if((strlen($tvn) > $ml) && (substr($tvn, 0, $ml) == $vn))
						{
							$pinf[$tpid]['data']['methode']		= (int)$tmp['methode'];
							$pinf[$tpid]['data']['zeitpunkt']	= (int)$tmp['zeitpunkt'];
							$pinf[$tpid]['data']['tage']		= (int)$tmp['tage'];
//echo('-&gt; Methode von ['.$vn.'] auf untergeordneten Vorgang ['.$tvn.'] übertragen.<br />');
						}
					}
				}
			}
			if($pinf[$pid]['data']['projekt_status'] == FXP_PS_COMPLETED)
				$pinf[$pid]['rfgrad']=1.00;
			else if($pinf[$pid]['data']['methode'] == 1784)	// Vergleich Soll-/Ist-Aufwand
				$pinf[$pid]['rfgrad']=$pinf[$pid]['sigrad'];
			else
			{
				if($pinf[$pid]['data']['projektart'] == FXP_PRJ_TASK)
					$pinf[$pid]['rfgrad']=MIN(0.9999, (double)$tmp['pk_fgrad']);
				else if($pinf[$pid]['aufwand_soll'] == 0.0)
					$pinf[$pid]['rfgrad']=0.000;
				else
				{
					// Geschätzer Ist-Aufwand von noch nicht abgeschlossenen Aufgaben, zu denen eine Schätzung vorhanden ist
					$sql  = "SELECT SUM(f.pk_fgrad * p.aufwand_soll) AS aufwand_ist_rel FROM projekte p, projektfortschritt f";
					$sql .= " WHERE f.projekt_id=p.projekt_id AND (f.personen_id IS NULL OR f.personen_id=0) AND f.methode<>1784 AND f.aktiv_kz=1";
					$sql .= " AND p.projekt_status<>".FXP_PS_COMPLETED." AND p.projekt_id IN (SELECT projekt_id FROM projekte WHERE projektart=".FXP_PRJ_TASK." AND vorgangsnummer LIKE '".$pinf[$pid]['data']['vorgangsnummer'].".%' AND mandanten_id=".$mid.")";
					$sql .= " AND p.mandanten_id=".$mid." AND f.mandanten_id=".$mid;
					$aufwand_ist_rel=get_double(db_value($sql));
//fxDebug($aufwand_ist_rel, $sql);
					// Tatsächlicher Ist-Aufwand von noch nicht abgeschlossenen Aufgaben, zu denen keine Schätzung vorhanden ist
					$sql  = "SELECT SUM(aufwand_ist) AS aufwand_ist_act FROM projekte";
					$sql .= " WHERE projektart=".FXP_PRJ_TASK." AND vorgangsnummer LIKE '".$pinf[$pid]['data']['vorgangsnummer'].".%' AND mandanten_id=".$mid;
					$sql .= " AND projekt_status<>".FXP_PS_COMPLETED." AND projekt_id NOT IN (SELECT p.projekt_id FROM projekte p, projektfortschritt f WHERE f.projekt_id=p.projekt_id AND (f.personen_id IS NULL OR f.personen_id=0) AND f.methode<>1784 AND f.aktiv_kz=1 AND p.mandanten_id=".$mid." AND f.mandanten_id=".$mid.")";
					$aufwand_ist_act=get_double(db_value($sql));
//fxDebug($aufwand_ist_act, $sql);
					// Tatsächlicher Ist-Aufwand von noch nicht abgeschlossenen Aufgaben, zu denen keine Schätzung vorhanden ist
					$sql  = "SELECT SUM(aufwand_soll) AS aufwand_ist_fertig FROM projekte";
					$sql .= " WHERE projektart=".FXP_PRJ_TASK." AND vorgangsnummer LIKE '".$pinf[$pid]['data']['vorgangsnummer'].".%' AND mandanten_id=".$mid;
					$sql .= " AND projekt_status=".FXP_PS_COMPLETED;
					$aufwand_ist_fertig=get_double(db_value($sql));
//fxDebug($aufwand_ist_fertig, $sql);

					$pinf[$pid]['rfgrad']=MIN(0.9999, ($aufwand_ist_rel+$aufwand_ist_act+$aufwand_ist_fertig)/$pinf[$pid]['aufwand_soll']);
				}
			}
		}
	}

//fxDebug($pinf, '$pinf');
	return $pinf;
}

/**
 * =============================================================================
 * PROJEKTFORTSCHRITT
 * =============================================================================
 * Funktion ermittelt, berechnet und korrigiert den Projektfortschritt
 * (= Fertigstellungsgrad) einer übergebenen Aufgabe bzw. setzt eine evtl.
 * neue Methode.
 * -----------------------------------------------------------------------------
 * Übergabeparameter:
 * °°°°°°°°°°°°°°°°°°
 * $pid			Projekt-ID
 * $pers		Personen-ID
 * $wert		Neue eingegebener Wert je Methode
 * $methode		Methode zur Berechnung des rel. Fortschrittgrades
 * $zeitpunkt	Zeitpunkt, wann der Fortschritt gemeldet werden soll
 * $tage		Falls Zeitpunkt Tage beinhaltet stehen hier die Tage
 * $part		Projektart, falls 0 wird die Art ermittelt (ausser bei Neuanl.)
 * $anm			Anmerkung
 * -----------------------------------------------------------------------------
 * Rückgabewert:
 * °°°°°°°°°°°°°
 * Fehlercode im Fehlerfall falls DB-Calls schiefgegangen sind, sonst false
 *
 * @param various $pid       - Mandatory parameter:                 ???PARAMETER???
 * @param integer $pers      - Optional parameter (default = 0):    ???PARAMETER???
 * @param double  $wert      - Optional parameter (default = 0.0):  ???PARAMETER???
 * @param integer $methode   - Optional parameter (default = 0):    ???PARAMETER???
 * @param integer $zeitpunkt - Optional parameter (default = 0):    ???PARAMETER???
 * @param integer $tage      - Optional parameter (default = 0):    ???PARAMETER???
 * @param integer $part      - Optional parameter (default = 0):    ???PARAMETER???
 * @param string  $anm       - Optional parameter (default = ''):   ???PARAMETER???
 *
 * @return ???RETURN???
 */
function projektfortschritt($pid, $pers=0, $wert=0.0, $methode=0, $zeitpunkt=0, $tage=0, $part=0, $anm='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pid=(int)$pid;
	if(!$pid)
		return 1805;	// Es wurde keine Projekt-ID übermittelt oder die Projekt-ID ist 0!

	$mid=$GLOBALS['fxpglobals']['client'];

	$part=(int)$part;
	if(!$part)
		$part=(int)db_value("SELECT projektart FROM projekte WHERE projekt_id=".$pid." AND mandanten_id=".$mid);
	if(!$part)
		return 1806;	// Der Vorgang besitzt noch keine Projektart!

	// Alte Werte ermitteln
	$sql ='';

	$chk ="SELECT id, pk_fgrad, methode, zeitpunkt, tage";
	$chk .= " FROM projektfortschritt";
	$chk .= " WHERE projekt_id=".$pid." AND mandanten_id=".$mid." AND (personen_id IS NULL OR personen_id=0) AND aktiv_kz=1";
	$pfs=db_values($chk);
	if(is_array($pfs))
	{
		$pfid=(int)$pfs['id'];
		$pfmt=(int)$pfs['methode'];
		$pfzp=(int)$pfs['zeitpunkt'];
		$pftg=(int)$pfs['tage'];
		// Änderung
		if(($methode && ($methode != $pfmt)) || ($zeitpunkt && ($zeitpunkt != $pfzp)) || ($tage && ($tage != $pftg)))
		{
			$sql="UPDATE projektfortschritt SET aktiv_kz=1";
			if($methode && ($methode != $pfmt))
			{
				$pfmt=(int)$methode;
				$sql .= ", methode=".$pfmt;
			}
			if($zeitpunkt && ($zeitpunkt != $pfzp))
			{
				$pfzp=(int)$zeitpunkt;
				$sql .= ", zeitpunkt=".$pfzp;
			}
			if($tage && ($tage != $pftg))
			{
				$pftg=(int)$tage;
				$sql .= ", tage=".$pftg;
			}
			$sql .= " WHERE id=".$pfid." AND mandanten_id=".$mid;
		}
	}
	// Neuanlage
	else if($methode)
	{
		$pfid=(int)id_max('projektfortschritt', 'id');
		$pfmt=(int)$methode;
		$pfzp=(int)$zeitpunkt;
		$pftg=(int)$tage;

		if(!$pfzp)
			$pfzp=1790;	// Auf Anforderung (Standard)

		$sql  = "INSERT INTO projektfortschritt";
		$sql .= " (mandanten_id, id, projekt_id, personen_id, pk_fgrad, methode, zeitpunkt, tage, aufwand_soll, aufwand_ist, allg_zahlenfeld, anmerkung, angelegt_von, anlage_dtm, aktiv_kz)";
		$sql .= " VALUES";
		$sql .= " (".$mid.", ".$pfid.", ".$pid.", NULL, 0.0, ".$pfmt.", ".$pfzp.", ".$pftg.", NULL, NULL, NULL, NULL, NULL, NULL, 1)";
	}
	else
		return 1807;	// Der Vorgang besitzt noch keine Berechnungsmethode für den Fertigstellungsgrad!

	// Globaler Projektsatz
	if($sql)
	{
		$err=db_x($sql);
//fxDebug($err, $sql);
		if($err)
			return 1808;	// Die Berechnungsmethode für den Fertigstellungsgrad konnte nicht angepasst werden!
	}

	// Kein globaler Fortschrittsatz vorhanden, dann mit Fehler beenden
	if(!$pfid)
		return 1807;	// Der Vorgang besitzt noch keine Berechnungsmethode für den Fertigstellungsgrad!

	// Projektfortschritt nur bei Aufgaben, sonst kann hier bereits Funktion erfolgreich verlassen werden
	if($part != FXP_PRJ_TASK)
		return false;

	// Basisinformationen ermitteln
	$pinfo=array('aufwand_soll'=>0.0, 'aufwand_ist'=>0.0, 'pers'=>array());

	// ...Elter, Projektleiter, Stellvertreter + Soll-Aufwand
	$sql="SELECT elter, projektleiter, vertreter, aufwand_soll FROM projekte WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
	$tmp=db_values($sql);
	$elter=(int)$tmp['elter'];
	$pl=(int)$tmp['projektleiter'];
	$sp=(int)$tmp['vertreter'];
	$pinfo['aufwand_soll']=$tmp['aufwand_soll'];
	// ...Zugeord. Personen
	$akt_ress=false;
	$sql="SELECT personen_id, prozent FROM maplanung WHERE projekt_id=".$pid." AND mandanten_id=".$mid." AND personen_id IS NOT NULL AND personen_id>0";
	$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql);
	if(is_array($tmp))
	{
		$akt_ress=true;
		foreach($tmp as $t)
			$pinfo['pers'][(int)$t['personen_id']]=array('id'=>0, 'ida'=>0, 'rfg'=>0.0, 'prozent'=>$t['prozent'], 'aufwand_ist'=>0.0);
	}
	else if($pl)
	{
		if($sp && ($sp == $pers))
			$pers=$pl;
		$pinfo['pers'][$pl]=array('id'=>0, 'ida'=>0, 'rfg'=>0.0, 'prozent'=>1.00, 'aufwand_ist'=>0.0);
	}
	// ...Bereits erfasste Schätzungen
	$sql="SELECT id, personen_id, pk_fgrad FROM projektfortschritt WHERE projekt_id=".$pid." AND mandanten_id=".$mid." AND personen_id IS NOT NULL AND personen_id>0 AND aktiv_kz=1";
	$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql);
	if(is_array($tmp))
	{
		foreach($tmp as $t)
		{
			$id=(int)$t['personen_id'];
			if(!$akt_ress)
			{
				if(($id == $pl) || ($id == $sp))	// Projektleiter oder Stellvertreter
					$id=$pl;
				else
					$id=-1;
			}
			if(is_array($pinfo['pers'][$id]))
			{
				$pinfo['pers'][$id]['id']	= $t['id'];
				$pinfo['pers'][$id]['rfg']	= $t['pk_fgrad'];
			}
		}
	}
	// ...Offene Anforderungen
	$sql="SELECT id, personen_id FROM projektfortschritt WHERE projekt_id=".$pid." AND mandanten_id=".$mid." AND personen_id IS NOT NULL AND personen_id>0 AND allg_zahlenfeld IS NULL AND angelegt_von IS NOT NULL AND anlage_dtm IS NOT NULL AND aktiv_kz=0";
	$tmp=db_values($sql, '*');
//fxDebug($tmp, 'APFF: '.$sql);
	if(is_array($tmp))
	{
		foreach($tmp as $t)
		{
			$id=(int)$t['personen_id'];
			if(!$akt_ress)
			{
				if(($id == $pl) || ($id == $sp))	// Projektleiter oder Stellvertreter
					$id=$pl;
				else
					$id=-1;
			}
			if(is_array($pinfo['pers'][$id]))
				$pinfo['pers'][$id]['ida']=$t['id'];
		}
	}

	// ...Zeiterfassung
	$sql="SELECT personen_id, zeitaufwand FROM zeitdaten WHERE projekt_id=".$pid." AND mandanten_id=".$mid." AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.")";
	$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql);
	if(is_array($tmp))
	{
		foreach($tmp as $t)
		{
			$id=(int)$t['personen_id'];
			if(is_array($pinfo['pers'][$id]))
				$pinfo['pers'][$id]['aufwand_ist']	+= $t['zeitaufwand'];

			$pinfo['aufwand_ist']	+= $t['zeitaufwand'];
		}
	}
//fxDebug($pinfo, $pid);

	if(!$wert)
		$wert=0.0;
	$owert=$wert;

	// Fertigstellungsgrad ermitteln
	$fg=0.0;
	if($pfmt == 1784)	// Vergleich Soll-/Ist-Aufwand
	{
		if($pinfo['aufwand_soll'] > 0.0)
			$fg=min(1.00, $pinfo['aufwand_ist'] / $pinfo['aufwand_soll']);
		else
			$fg=1.00;
	}
	else if(is_array($pinfo['pers']))
	{
		foreach($pinfo['pers'] as $id => $p)
		{
			$gfg=0.0;
			$rfg=0.0;
			if($id == $pers)
			{
				switch($pfmt)
				{
					case 1785:	// Restaufwand in Stunden => Wert muss aber in Sekunden der Funktion übergeben werden
						$wert=max(0.0, $wert);
						// Kein Restaufwand, d.h. die Aufgabe ist zu 100% fertig
						if($wert == 0.0)
							$gfg=1.00;
						// Falls bereits Zeiterfassung vorliegt spielt der Sollaufwand keine Rolle (lt. vorgeschl. Formel)
						else if($p['aufwand_ist'])
							$gfg=$p['aufwand_ist'] / ($p['aufwand_ist'] + $wert);
						// Falls kein Sollaufwand vorliegt oder der Sollaufwand kleiner als der geschätzte Restaufwand ist, dann wurde die Aufgabe nocht nicht begonnen
						else if(!$pinfo['aufwand_soll'] || (($pinfo['aufwand_soll']*$p['prozent']) <= $wert))
							$gfg=0.0;
						else
							$gfg=(($pinfo['aufwand_soll']*$p['prozent']) - $wert) / ($pinfo['aufwand_soll']*$p['prozent']);
					break;

					case 1786:	// Methode 0/50/100
						$wert=min(100.0, max(0.0, $wert));
						if($wert >= 100.0)
							$wert=1.00;
						else if($wert > 0.0)
				    		$wert=0.50;
			    		else
					    	$wert=0.0;
						$gfg=$wert;
					break;

					case 1787:	// Methode 0/100
						$wert=min(100.0, max(0.0, $wert));
						if($wert >= 100.0)
							$wert=1.00;
						else
							$wert=0.0;
						$gfg=$wert;
					break;

					case 1788:	// Prozentsatz Restaufwand
						$wert=min(100.0, max(0.0, $wert));
						$gfg=(1.00-($wert/100.0));
					break;

					case 1789:	// Prozentsatz Fertigstellung
						$wert=min(100.0, max(0.0, $wert));
						$gfg=$wert/100.0;
					break;
				}
				$rfg=$gfg*$p['prozent'];
				$fg += $rfg;
//echo('<b>projektfortschritt</b>($pid='.$pid.', $pers='.$pers.')<br />$pfmt='.$pfmt.', $wert='.$wert.', $gfg='.$gfg.'<br />$rfg='.$rfg.', $fg='.$fg.'<hr />');

				// Existiert eine alte Fortschrittseingabe der Person, wenn ja dann inaktiv setzen
				if($p['id'])
				{
					$sql="UPDATE projektfortschritt SET aktiv_kz=0 WHERE id=".$p['id']." AND mandanten_id=".$mid;
					$err=db_x($sql, 0, false, false);	// Änderungs_ID + Zeitstempel nicht ändern
//fxDebug($err, $sql);
					if($err)
						return 1809;	// Der vorhandene geschätzte Fertigstellungsgrad konnte nicht verändert werden!
				}

				if(!strlen($anm))
					$anm='NULL';
				else
					$anm="'".convert_string(trim($anm), 'todb')."'";
				// Ist dies eine Schätzung auf eine Anforderung? Wenn ja, dann Satz updaten und keinen neuen anlegen
				if($p['ida'])
				{
					$sql  = "UPDATE projektfortschritt";
					$sql .= " SET pk_fgrad=".$gfg.", methode=".$pfmt.", zeitpunkt=".$pfzp.", tage=".$pftg.",";
					$sql .= " aufwand_soll=".($pinfo['aufwand_soll']*$p['prozent']).", aufwand_ist=".$p['aufwand_ist'].",";
					$sql .= " allg_zahlenfeld=".$owert.", anmerkung=".$anm.", aktiv_kz=1";
					$sql .= " WHERE id=".$p['ida']." AND mandanten_id=".$mid;
				}
				// sonst neue Fortschrittseingabe der Person speichern
				else
				{
					$fid=(int)id_max('projektfortschritt', 'id');

					$sql  = "INSERT INTO projektfortschritt";
					$sql .= " (mandanten_id, id, projekt_id, personen_id, pk_fgrad, methode, zeitpunkt, tage, aufwand_soll, aufwand_ist, allg_zahlenfeld, anmerkung, angelegt_von, anlage_dtm, aktiv_kz)";
					$sql .= " VALUES";
					$sql .= " (".$mid.", ".$fid.", ".$pid.", ".$id.", ".$gfg.", ".$pfmt.", ".$pfzp.", ".$pftg.", ".($pinfo['aufwand_soll']*$p['prozent']).", ".$p['aufwand_ist'].", ".$owert.", ".$anm.", ";
					if($id != $GLOBALS['fxpglobals']['person'])
						$sql .= $GLOBALS['fxpglobals']['person'];
					else
						$sql .= "NULL";
					$sql .= ", NULL, 1)";
				}
				$err=db_x($sql);
//fxDebug($err, $sql);
				if($err)
					return 1810;	// Der neue geschätzte Fertigstellungsgrad konnte nicht erstellt werden!
			}
			else
				$fg += $p['rfg'];
		}
	}

	// Fertigstellungsgrad der Aufgabe speichern
	$sql="UPDATE projektfortschritt SET pk_fgrad=".$fg." WHERE id=".$pfid." AND mandanten_id=".$mid;
	$err=db_x($sql);
//fxDebug($err, $sql);
	if($err)
		return 1811;	// Der neu errechnete Fertigstellungsgrad konnte nicht angepasst werden!

	return false;
}

/**
 * =============================================================================
 * GET_PMETHOD
 * =============================================================================
 * Funktion ermittelt die zu verwendende Methode sowie den Zeitpunkt + Tage
 * zur Berechnung des Projektfortschritts (=Fertigstellungsgrad).
 * -----------------------------------------------------------------------------
 * Übergabeparameter:
 * °°°°°°°°°°°°°°°°°°
 * $pid			Projekt-ID
 * -----------------------------------------------------------------------------
 * Rückgabewert:
 * °°°°°°°°°°°°°
 * false im Fehlerfall, sonst Array mit Methode, Zeitpunkt und Tage
 *
 * @param various $pid - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_pmethod($pid)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pid	= (int)$pid;
	$rempid	= array();
	$arr	= '';

	while($pid && !is_array($arr))
	{
		if($rempid[$pid])
			break;
		$rempid[$pid]=true;

		$sql="SELECT methode, zeitpunkt, tage FROM projektfortschritt WHERE projekt_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND (personen_id IS NULL OR personen_id=0) AND aktiv_kz=1";
		$arr=db_values($sql);
//fxDebug($arr, $sql);
		if(!is_array($arr))
		{
			$sql="SELECT elter FROM projekte WHERE projekt_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$pid=(int)db_value($sql);
//fxDebug($pid, $sql);
		}
	}

	if(!is_array($arr))
		$arr=array('methode'=>1784, 'zeitpunkt'=>1790, 'tage'=>7);

	return $arr;
}

/**
 * ???FUNCTION???
 *
 * @param string  $pids     - Optional parameter (default = ''):    ???PARAMETER???
 * @param string  $sqladdon - Optional parameter (default = ''):    ???PARAMETER???
 * @param boolean $retsql   - Optional parameter (default = false): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_methode($pids='', $sqladdon='', $retsql=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!$retsql)
	{
		$pinf=array();

		$sqlz=" FROM projektfortschritt f";
		$sqlz .= " WHERE (f.personen_id IS NULL OR f.personen_id=0) AND f.aktiv_kz=1";
		if(strlen($pids))
			$sqlz .= " AND f.projekt_id IN (".$pids.")";
		$sqlz .= " AND f.mandanten_id=".$GLOBALS['fxpglobals']['client'];
		if(strlen($sqladdon))
			$sqlz .= " ".$sqladdon;

		$sql="SELECT f.id, f.projekt_id, f.methode, f.zeitpunkt, f.tage".$sqlz;
		$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql);
		if(is_array($tmp))
		{
			$elter="SELECT f.projekt_id".$sqlz;

			foreach($tmp as $t)
			{
				// Aktuelle Informationen
				$apid=(int)$t['projekt_id'];

				$pinf[$apid]['methode']		= $t['methode'];
				$pinf[$apid]['zeitpunkt']	= $t['zeitpunkt'];
				$pinf[$apid]['tage']		= $t['tage'];
				$pinf[$apid]['elter']		= 0;
			}

			while(strlen($elter))
			{
				$sql="SELECT projekt_id, elter FROM projekte WHERE elter IN (".$elter.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql);
				$elter="";
				if(fxIsArray($tmp))
				{
					foreach($tmp as $t)
					{
						$apid=(int)$t['projekt_id'];
						$epid=(int)$t['elter'];

						if(!fxIsArray($pinf[$apid]) && $epid && fxIsArray($pinf[$epid]))
						{
							$pinf[$apid]			= $pinf[$epid];
							$pinf[$apid]['elter']	= $epid;

							if($elter)
								$elter .= ",";
							$elter .= $apid;
						}
					}
				}
			}
		}
	}
	else
	{
		$sql  = "SELECT f.projekt_id, p.vorgangsnummer";
		$sql .= " FROM projektfortschritt f, projekte p";
		$sql .= " WHERE p.projekt_id=f.projekt_id AND (f.personen_id IS NULL OR f.personen_id=0) AND f.aktiv_kz=1";
		if(strlen($pids))
			$sql .= " AND f.projekt_id IN (".$pids.")";
		$sql .= " AND f.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
		if(strlen($sqladdon))
			$sql .= " ".$sqladdon;
		$sql .= " ORDER BY p.vorgangsnummer";
		$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql);
		if(!is_array($tmp))
			return "-666";

		$vnrem=array();
		$vnsql="";
		foreach($tmp as $t)
		{
			$vn		= $t['vorgangsnummer'];
			$found	= false;
			if(sizeof($vnrem))
			{
				foreach($vnrem as $vncmp => $vnlen)
				{
					if(substr($vn, 0, $vnlen) === $vncmp)
					{
						$found=true;
						break;
					}
				}
			}
			if($found)
				continue;

			$vnrem[$vn]=strlen($vn);

			if(strlen($vnsql))
				$vnsql .= " OR ";
			$vnsql .= "vorgangsnummer LIKE '".$t['vorgangsnummer']."%'";
		}

		$pinf="SELECT projekt_id FROM projekte WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND (".$vnsql.")";
	}
//fxDebug($pinf, '$pinf');

	return $pinf;
}

/**
 * ???FUNCTION???
 *
 * @param various $pid - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_projekt_methode($pid)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pinf=array('methode'=>1784, 'zeitpunkt'=>1790, 'tage'=>7);
	$pid =(int)$pid;
	if(!$pid || ($pid < 1))
		return $pinf;

	$sqlt="SELECT methode, zeitpunkt, tage FROM projektfortschritt WHERE (personen_id IS NULL OR personen_id=0) AND aktiv_kz=1 AND projekt_id=";
	while(true)
	{
		$sql =$sqlt.$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$tmp=db_values($sql);
//fxDebug($tmp, $sql);
		if(fxIsArray($tmp))
			return $tmp;
		else
		{
			$sql="SELECT elter FROM projekte WHERE projekt_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$pid=(int)db_value($sql);
//fxDebug($pid, $sql);
			if(!$pid)
				return $pinf;
		}
	}
}

/**
 * wie inferior nur schneller, da auf Verzeichnis zugegriffen wird anstatt der Suche nach Elter
 *
 * @param various $Projekt_ID - Mandatory parameter:                        ???PARAMETER???
 * @param integer $mode       - Optional parameter (default = 0):           ???PARAMETER???
 * @param string  $level      - Optional parameter (default = ''):          ???PARAMETER???
 *
 * @return ???RETURN???
 */
function inferior_vn($Projekt_ID, $mode=0, $level='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(fxIsArray($Projekt_ID))
	{
		$pid='';
		foreach($Projekt_ID as $id)
		{
			if($pid)
				$pid .= ',';
			$pid .= (int)$id;
		}
		$Projekt_ID=$pid;
	}

	// Fehler abfangen
	if(!strlen($Projekt_ID) || ($Projekt_ID < 1))
		return "-666";

	// Variablen initialisieren
	if(substr($Projekt_ID, 0, 1) == ',')
		$Projekt_ID=substr($Projekt_ID, 1);

	// Vorgangsnummer der übergebenen Vorgänge ermitteln
	$vn=db_values("SELECT vorgangsnummer FROM projekte WHERE projekt_id IN (".$Projekt_ID.") AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY vorgangsnummer", '*');
	if(!fxIsArray($vn))
		return "-666";

	$vn_string="";
	foreach($vn as $vnummer)
	{
		// Wenn Mutterprojekt bereits im String => keine weiteren Unterprojekte mehr nötig
		if(!strstr($vn_string, "'".substr($vnummer['vorgangsnummer'], 0, 4)."%'"))
			$vn_string .= " OR vorgangsnummer LIKE '".$vnummer['vorgangsnummer']."%'";
	}
	$vn_string=" AND (".substr($vn_string, 4).")";

	$pas="";
	if(($mode == FXP_DISPLAY) || ($mode == FXP_CHANGE))	// (Teil-)Projekte
		$pas .= ",".FXP_PRJ_MAIN.",".FXP_PRJ_SUB;
	if(($mode == FXP_CREATE) || ($mode == FXP_CHANGE))	// Aufgabe
		$pas .= ",".FXP_PRJ_TASK;
	if(strlen($pas))
		$vn_string .= " AND projektart IN (".substr($pas, 1).")";
	else
		$vn_string .= " AND projektart IN (".FXP_PRJ_MAIN.",".FXP_PRJ_SUB.",".FXP_PRJ_TASK.")";

	$sql="SELECT projekt_id FROM projekte WHERE mandanten_id=".$GLOBALS['fxpglobals']['client'].$vn_string;
//echo('<B>inferior_vn:</B> &quot;'.$sql.'&quot;<br />'.$GLOBALS['nl']);
	return $sql;
}

/**
 * Liefert einen SQL aller übergeordneten Projekt-ID´s inkl. Aufrufs-ID, falls gewünscht.
 * Im Fehlerfall oder falls kein übergeodnetes Projekt vorhanden ist wird der String "-666" zurückgeliefert.
 *
 * @param various $pid         - Mandatory parameter:                  ???PARAMETER???
 * @param boolean $with_origin - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function superior_vn($pid, $with_origin=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(is_array($pid))
	{
		$pids='';
		foreach($pid as $id)
		{
			if(strlen($pids))
				$pids .= ',';
			$pids .= (int)$id;
		}
		$pid=$pids;
	}

	// Fehler abfangen
	if(!strlen($pid) || ($pid < 1))
		return "-666";

	// Vorgangsnummer ermitteln
	$sql="SELECT vorgangsnummer FROM projekte WHERE projekt_id IN (".$pid.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$vna=db_values($sql, '*');
	if(!is_array($vna))
		return "-666";

	$sql="";
	if($with_origin && (sizeof($vna) == 1))
		$sql="vorgangsnummer='".$vna[0]['vorgangsnummer']."'";

	foreach($vna as $vnt)
	{
		$vn=substr($vnt['vorgangsnummer'], 0, -5);
		while(strlen($vn) > 3)
		{
			if(strlen($vn) < 4)
				break;

			if(strlen($sql))
				$sql .= " OR ";
			$sql .= "vorgangsnummer='".$vn."'";

			$vn=substr($vn, 0, -5);
		}
	}

	if(!strlen($sql))
		return "-666";

	$sql="SELECT projekt_id FROM projekte WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND (".$sql.")";
//echo('<B>superior_vn:</B> &quot;'.$sql.'&quot;<br />'.$GLOBALS['nl']);
	return $sql;
}

/**
 * Diese Funktion erstellt bei Übergabe einer Projekt_ID (i.d.R. Mutterprojekt) das
 * Verzeichnis (Vorgangsnummern) des Projektastes. Zu beachten ist, das der übergebenene
 * Vorgang bereits einen Vorgangsnummer haben muss. Handelt es sich beim übergebenen
 * Vorgang um ein Mutterprojekt, so sollte diese Vorgangsnummer eine Ganzzahl sein.
 * verzeichnis($Projekt_ID);
 * Parameter:
 * Projekt_ID: ab diesem Projekt werden alle Untervorgänge mit evtl. neuer Nr. versehen
 * Author: Stefan (Original) + wtj (Neue Version)
 *
 * @param various $Projekt_ID - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function verzeichnis($Projekt_ID)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	//echo('<B>verzeichnis</B>($Projekt_ID='.$Projekt_ID.');<br />');
	$Projekt_ID=(int)$Projekt_ID;
	if(!$Projekt_ID)
		return true;

	// Vorgangsnummer und Projektart des übergebenen Vorgangs bestimmen
	$sql="SELECT vorgangsnummer, projektart, elter FROM projekte WHERE projekt_id=".$Projekt_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$va_array=db_values($sql);
//fxDebug($va_array, $sql);
	$vn =$va_array['vorgangsnummer'];
	$art=(int)$va_array['projektart'];
	$etr=(int)$va_array['elter'];
	if(!strlen($vn) || !$art)
		return true;

	$chkp=array();
	$arrp=array();

	// ...Mutterprojekt
	if(!$etr || ($art == FXP_PRJ_MAIN))
	{
		if(strlen($vn) != 4)
		{
			$vnm=(int)substr(db_value("SELECT vorgangsnummer FROM projekte WHERE projektart=".FXP_PRJ_MAIN." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY vorgangsnummer DESC"), 0, 4);
			if($vnm)
			{
				$vnm++;
				if($vnm > 9999)
					return true;
			}
			else if(strtolower(substr($GLOBALS['fxpglobals']['settings']['vorgang0'], 0, 1)) == 'n')
				$vnm=1;
			else
				$vnm=0;
			$vnm=substr('0000'.(int)$vnm, -4);

			$usql="UPDATE projekte SET vorgangsnummer='".$vnm."' WHERE projekt_id=".$Projekt_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
//fxDebug($usql, '$usql');
			$err=db_x($usql);
			if($err)
				return $err;
		}
	}
	// ...Aufgabe
	else if($art == FXP_PRJ_TASK)
	{
		// Existiert eine Aufgabe mit gleicher Vorgangsnummer?
		$aex=exist_vn($Projekt_ID, $vn);
		if(!$aex)
			return false;

		// Ja, dann übergeordnetes Projekt prüfen
		$Projekt_ID=$etr;
	}
	$chkp[$Projekt_ID]=db_value("SELECT vorgangsnummer FROM projekte WHERE projekt_id=".$Projekt_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);;
	$arrp[$chkp[$Projekt_ID]]=array('id'=>$Projekt_ID, 'vno'=>$chkp[$Projekt_ID]);

	// Untergeordnete Vorgänge bestimmen
	while(sizeof($chkp))
	{
//fxDebug($chkp, '$chkp');
		$nchkp=array();
		foreach($chkp as $elt => $vne)
		{
			$sql="SELECT projekt_id, projektart, vorgangsnummer FROM projekte WHERE elter=".$elt." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY vorgangsnummer ASC, zeitstempel";
			$arr=db_values($sql, '*');
//fxDebug($arr, $sql);
			if(is_array($arr))
			{
				foreach($arr as $a)
				{
					$pid	= (int)$a['projekt_id'];
					$vno	= $a['vorgangsnummer'];
					$vnl	= (int)substr($vno, -4);
					if(!$vnl && (strtolower(substr($GLOBALS['fxpglobals']['settings']['vorgang0'], 0, 1)) == 'n'))
						$vnl=1;
					$vnn	= $vne.'.'.substr('0000'.$vnl, -4);

					if($a['projektart'] == FXP_PRJ_SUB)
						$nchkp[$pid]=$vnn;

					if(!is_array($arrp[$vnn]))
						$arrp[$vnn]=array('id'=>$pid, 'vno'=>$vno);
					else
					{
						$emc=0;
						while(true)
						{
							$vnl=1+(int)substr($vnn, -4);
							$emc++;
							if(($vnl > 9999) || ($emc > 9999))	//Emergency-Break
								return true;
							$vnn=substr($vnn, 0, -4).substr('0000'.$vnl, -4);
							if(!is_array($arrp[$vnn]))
							{
								$arrp[$vnn]=array('id'=>$pid, 'vno'=>$vno);
								break;
							}
						}
					}
				}
			}
		}

		$chkp=$nchkp;
	}

	if(sizeof($arrp))
	{
//fxDebug($arrp, '$arrp');
		foreach($arrp as $vnn => $p)
		{
			// Hat sich die Vorgangsnummer geändert, dann neue Nummer in DB schreiben
			if($p && ((string)$vnn !== (string)$p['vno']))
			{
				$usql="UPDATE projekte SET vorgangsnummer='".$vnn."' WHERE projekt_id=".$p['id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
//fxDebug($usql, '$usql');
				$err=db_x($usql);
				if($err)
					return $err;
			}
		}
	}

	return false;
}

/**
 * Diese Funktion überprüft bei Übergabe einer Projekt-ID und einer Vorgangsnummer, ob
 * diese Vorgangsnummer bereits belegt ist. Ist dem so, wird die Projekt-ID dieses Vorgangs
 * zurückgeliefert
 *
 * @param various $Projekt_ID - Mandatory parameter: ???PARAMETER???
 * @param various $VN         - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function exist_vn($Projekt_ID, $VN)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$Projekt_ID	= (int)$Projekt_ID;
	$exists		= (int)db_value("SELECT projekt_id FROM projekte WHERE vorgangsnummer='".$VN."' AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND projekt_id<>".$Projekt_ID);

	return $exists;
}

/**
 * Ermittelt, ob die Nummerierung der Vorgänge bei 0, oder 1 beginnt
 * @return int 0, wenn die Nummerierung bei 0 beginnt, sonst 1
 */
function getFirstVNNumber()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(strtoupper(substr($GLOBALS['fxpglobals']['settings']['vorgang0'], 0, 1)) == 'J')
		return 0;
	else
		return 1;
}

/**
 * Berechnet Aufwand_Soll neu, wenn ein Vorgang verschoben wird
 * Author: Stefan
 *
 * @param various $Elter_s         - Mandatory parameter:               ???PARAMETER???
 * @param integer $Aufwand_Soll_up - Optional parameter (default = 0):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function Verschiebung($Elter_s, $Aufwand_Soll_up=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Keine oder fehlerhafte ID übergeben, dann beenden
	if($Elter_s < 1)
		return false;
	// Nichts zu tun, dann gleich beenden
	$Aufwand_Soll_up=(int)$Aufwand_Soll_up;
	if(!$Aufwand_Soll_up)
		return false;

	$z_err=false;
	while($Elter_s)
	{
		// Berechnung für Projekte
		$sql="SELECT elter, aufwand_soll FROM projekte WHERE projekt_id=".$Elter_s." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$Projekt=db_values($sql);
		if(fxIsArray($Projekt))
		{
			if(!$z_err)
			{
				$Aufwand_Soll=(int)$Projekt['aufwand_soll'] + $Aufwand_Soll_up;
				if($Aufwand_Soll < 0)
					$Aufwand_Soll=0;
				$sql="UPDATE projekte SET aufwand_soll=".$Aufwand_Soll." WHERE projekt_id=".$Elter_s." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$z_err=db_x($sql);
			}

			// Nächstes Projekt
			if(!$z_err && $Projekt['elter'])
				$Elter_s=$Projekt['elter'];
			else
				break;
		}
		else
			break;
	}

	return $z_err;
}

/**
 * no_null
 * Diese Funktion entfernt die führenden Nullen bei der Anzeige der Vorgangs-
 * nummer.
 *
 * @param various $Vorgangsnummer - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function no_null($Vorgangsnummer)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Da die Vorgangsnummer in der DB mit 4 Nummern (bis zu 3 führende Nullen)
	// dargestellt wird, werden hier die Nullen einfach ausgeblendet
	$nvn=(int)substr($Vorgangsnummer, 0, 4);
	$pps=strpos($Vorgangsnummer, '.');
	if($pps > 0)
	{
		$vnr=array('.000'=>'.', '.00'=>'.', '.0'=>'.');
		$nvn=$nvn.strtr(substr($Vorgangsnummer, $pps), $vnr);
	}

	return $nvn;
}

/**
 * add_null
 * Diese Funktion wandelt jede Vorgangsnummer in das fx-project Format mit
 * führenden Nullen und 4-stellig um.
 *
 * @param various $Vorgangsnummer - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function add_null($Vorgangsnummer)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$va=explode('.', $Vorgangsnummer);
	if(is_array($va))
	{
		$nvn=substr('0000'.(int)$va[0], -4);
		foreach($va as $vl => $lvn)
		{
			if($vl)
				$nvn .= '.'.substr('0000'.(int)$lvn, -4);
		}
	}
	else
		$nvn=$Vorgangsnummer;

	return $nvn;
}

/**
 * set_status
 * Bei Übergabe der Projekt_ID und des neuen Status, wird das aktuelle Projekt
 * mit dem neuen Status versehen, u.U. auch einigealle Übergeordneten.
 *
 * @param various $pid         - Mandatory parameter:                   ???PARAMETER???
 * @param integer $new_status  - Optional parameter (default = 297):    ???PARAMETER???
 * @param boolean $subprojects - Optional parameter (default = true):   ???PARAMETER???
 * @param boolean $sendmail    - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function set_status($pid, $new_status=297, $subprojects=true, $sendmail=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	//echo('<b>set_status('.$pid.', '.$new_status.');</b><br />');
	$pid=(int)$pid;
	if(!$pid || ($pid < 1))
		return true;

	$cid=$GLOBALS['fxpglobals']['client'];
	$new_status=(int)$new_status;

	// Vorgangsnummer bestimmen
	$sql="SELECT vorgangsnummer FROM projekte WHERE projekt_id=".$pid." AND mandanten_id=".$cid;
	$vn =trim(db_value($sql));
//fxDebug($vn, $sql);
	if(!strlen($vn))
		return true;

	$vn_inxt=$vn;
	$vn_enxt=$vn;
	if($subprojects)
	{
		$vn_inxt .= "%";
		$vn_enxt .= ".%";
	}

	$vn_prv="";
	if(strlen($vn) > 5)
	{
		$vnp=$vn;
		while(true)
		{
			$vnp=substr($vnp, 0, -5);
			if(strlen($vn_prv))
				$vn_prv .= " OR ";
			$vn_prv .= "vorgangsnummer='".$vnp."'";
			if(strlen($vnp) < 5)
				break;
		}
	}
//echo('$vn_prv=['.$vn_prv.'], $vn=['.$vn.']<br />');

	$aarr=array();

	// Alle untergeordneten Aufgaben mit neuem Status ermitteln
	if($subprojects)
	{
		$sql  = "SELECT projekt_id, projekt_status FROM projekte";
		$sql .= " WHERE vorgangsnummer LIKE '".$vn_enxt."' AND projektart=".FXP_PRJ_TASK." AND mandanten_id=".$cid." AND projekt_status<>".$new_status;
		if($new_status != FXP_PS_COMPLETED)
			$sql .= " AND projekt_status<>".FXP_PS_COMPLETED;
		$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql);
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
			{
				$tpid=(int)$t['projekt_id'];
				$tpst=(int)$t['projekt_status'];

				$aarr[$tpid]=$tpst;
			}
		}
	}

	$rerr=false;

	// Beim Abschliessen eines Projektes nur untergeordnete Teilprojekte/Aufgaben abschliessen, welche bereits aktiviert wurden
	if($new_status == FXP_PS_COMPLETED)
		$sql_status="=".FXP_PS_ACTIVE;
	// Den aktuellen sowie alle untergeordneten Vorgänge auf neuen Status setzen, außer denen die bereits den neuen Status haben oder "abgeschlossen" sind
	else
		$sql_status="<>".FXP_PS_COMPLETED;
	$bed=" WHERE vorgangsnummer LIKE '".$vn_inxt."' AND projekt_status<>".$new_status." AND projekt_status".$sql_status." AND mandanten_id=".$cid;
	$sql="SELECT COUNT(projekt_id) FROM projekte".$bed;
	$cnt=(int)db_value($sql);
//fxDebug($cnt, $sql);
	if($cnt)
	{
		$sql  = "UPDATE projekte SET projekt_status=".$new_status.$bed;
		$rerr=db_x($sql);
//fxDebug($sql, '$rerr: '.$rerr);
		if($rerr)
			return $rerr;
	}

	// Evtl. übergeordnete Vorgänge anpassen
	if(strlen($vn_prv))
	{
		// Beim Setzen auf "aktiv" werden dessen Elter auch automatisch aktiv
		if($new_status == FXP_PS_ACTIVE)
		{
			$bed =" WHERE (".$vn_prv.") AND projekt_status<>".$new_status." AND mandanten_id=".$cid;
			$ecnt=(int)db_value("SELECT COUNT(projekt_id) FROM projekte".$bed);
			if($ecnt)
			{
				$cnt += $ecnt;

				$sql  = "UPDATE projekte SET projekt_status=".$new_status.$bed;
				$rerr=db_x($sql);
//fxDebug($sql, '$rerr: '.$rerr);
			}
		}
		else
		{
			$sql  = "SELECT projekt_id FROM projekte";
			$sql .= " WHERE (".$vn_prv.") AND projekt_status<>".$new_status." AND mandanten_id=".$cid;
			$sql .= " ORDER BY vorgangsnummer DESC";
			$eids=db_values($sql, '*');
//fxDebug($eids, $sql);
			if(is_array($eids))
			{
				foreach($eids as $elt)
				{
					$eid=(int)$elt['projekt_id'];

					// Vorgänge ermitteln, die auf dieser Projektebene einen anderen Status haben, als den übergebenen.
					// Wird kein derartiges gefunden, kann der Elter auch auf den übergebenen Status gesetzt werden.
					$sql  = "SELECT projekt_status FROM projekte";
					$sql .= " WHERE elter=".$eid." AND projekt_id<>".$pid." AND projekt_status<>".$new_status." AND mandanten_id=".$cid;
					$est =(int)db_value($sql);
//fxDebug($est, $sql);
					if(!$est)
					{
						$cnt++;

						$sql  = "UPDATE projekte SET projekt_status=".$new_status;
						$sql .= " WHERE projekt_id=".$eid." AND mandanten_id=".$cid;
						$rerr=db_x($sql);
//fxDebug($sql, '$rerr: '.$rerr);
					}
					else
						break;
				}
			}
		}
	}

	// Bei Statusänderungen evtl. Ist-Zeiten setzen
	if(!$rerr)
		$rerr=ist_zeiten($pid);

	// Bei Abschluss von Vorgängen alle evtl. abhängigen nachfolgenden geplanten Vorgänge aktivieren
	if(!$rerr && ($new_status == FXP_PS_COMPLETED) && (strtolower($GLOBALS['fxpglobals']['settings']['aa_aktivieren']) == 'ja'))
	{
		$sql="SELECT projekt_id, projekt_status FROM projekte WHERE abhaengigkeit=".$pid." AND projekt_status=".FXP_PS_PLANNED." AND mandanten_id=".$cid;
		$tmp=db_values($sql, '*');
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
			{
				$tpid=(int)$t['projekt_id'];
				$tpst=(int)$t['projekt_status'];

				$sql  = "UPDATE projekte SET projekt_status=".FXP_PS_ACTIVE;
				$sql .= " WHERE projekt_id=".$tpid." AND mandanten_id=".$cid;
				$rerr=db_x($sql);
				if($rerr)
					break;

				$cnt++;
				$aarr[$tpid]=$tpst;
			}
		}
	}

	// Wurde der Status von Aufgaben geändert, dann Budget und Maplanung anpassen
	if(!$rerr && $cnt && sizeof($aarr))
	{
//fxDebug($aarr, 'Budget dieser Aufgaben ändern');
		foreach($aarr as $aid => $astat)
		{
			$rerr=update_budget($aid, array('dummy'=>0));
			if(!$rerr)
				$rerr=update_maplanung('person', $cid, $aid, '', '', false, true, false, $sendmail);
			if(!$rerr)
				$rerr=update_maplanung('sachmittel', $cid, $aid, '', '', false, true, false, $sendmail);
			if($rerr)
				break;
		}
	}

	return $rerr;
}

/**
 * ist_zeiten
 * Diese Funktion trägt entweder das Ist-Startdatum oder das Ist-Enddatum ein
 * (jeweils in die Aufgabe als auch in die der Aufgabe übergeordneten Vorgänge),
 * vorrausgesetzt, dass übergebene Datum ist vor bzw. nach dem bisher eingetra-
 * genen.
 * -----------------------------------------------------------------------------
 * ACHTUNG: Beim Ist-Enddatum müssen die Elter der Aufgabe bereits den Status
 * "abgeschlossen" erreicht haben, daher sollte zuvor die Funktion "set_status"
 * ausgeführt werden.
 *
 * @param various $pid - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function ist_zeiten($pid)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	//echo('<b>ist_zeiten('.$pid.');</b><br />');
	$pid=(int)$pid;
	if(!$pid || ($pid < 1))
		return true;

	$cid=$GLOBALS['fxpglobals']['client'];

	// Vorgangsnummer bestimmen
	$sql="SELECT vorgangsnummer FROM projekte WHERE projekt_id=".$pid." AND mandanten_id=".$cid;
	$vn =trim(db_value($sql));
//fxDebug($vn, $sql);
	if(!strlen($vn))
		return true;

	$vn_nxt	= " OR p.vorgangsnummer LIKE '".$vn.".%'";
	$vn_prv="";
	if(strlen($vn) > 5)
	{
		$vnp=$vn;
		while(true)
		{
			$vnp=substr($vnp, 0, -5);
			$vn_prv .= " OR p.vorgangsnummer='".$vnp."'";
			if(strlen($vnp) < 5)
				break;
		}
	}
//echo('$vn_prv=['.$vn_prv.'], $vn=['.$vn.'], $vn_nxt=['.$vn_nxt.']<br />');

	// Alle erforderlichen Projektdaten ermitteln
	$sql  = "SELECT p.projekt_id, p.vorgangsnummer, p.projektart, p.projekt_status, z.ist_beginn, z.ist_ende".$GLOBALS['nl'];
	$sql .= " FROM projekte p, zeitdaten z".$GLOBALS['nl'];
	$sql .= " WHERE z.projekt_id=p.projekt_id AND z.zeitart=102 AND p.mandanten_id=".$cid.$GLOBALS['nl'];
	$sql .= " AND (p.vorgangsnummer='".$vn."'".$vn_prv.$vn_nxt.") AND z.mandanten_id=".$cid.$GLOBALS['nl'];
	$sql .= " ORDER BY p.vorgangsnummer";
	$parr=db_values($sql, '*');
//fxDebug($parr, $sql);
	if(!is_array($parr))
		return false;

	// Änderungs-SQLs erstellen
	$sarr=array();
	foreach($parr as $pz => $pd)
	{
		$sql  = "SELECT MIN(ist_beginn) AS zmin, MAX(ist_ende) AS zmax".$GLOBALS['nl'];
		$sql .= " FROM zeitdaten".$GLOBALS['nl'];
		$sql .= " WHERE projekt_id IN (SELECT projekt_id FROM projekte WHERE vorgangsnummer LIKE '".$pd['vorgangsnummer']."%')";
		$sql .= " AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.",".FXP_TE_TRAVELEXP.") AND mandanten_id=".$cid;
		$zmm=db_values($sql);
//fxDebug($zmm, $sql);
		$zmin=trim($zmm['zmin']);
		$zmax=trim($zmm['zmax']);
		if($pd['projekt_status'] != FXP_PS_COMPLETED)
			$zmax='';

		// Ist-Beginn
		$ibc="";
		if($pd['ist_beginn'] != $zmin)
		{
			$ibc .= "ist_beginn=";
			if(strlen($zmin))
				$ibc .= "'".$zmin."'";
			else
				$ibc .= "NULL";
		}
		// Ist-Ende
		$iec="";
		if($pd['ist_ende'] != $zmax)
		{
			if(strlen($ibc))
				$iec .= ", ";

			$iec .= "ist_ende=";
			if(strlen($zmax))
				$iec .= "'".$zmax."'";
			else
				$iec .= "NULL";
		}

		if(strlen($ibc) || strlen($iec))
			$sarr[]="UPDATE zeitdaten SET ".$ibc.$iec." WHERE projekt_id=".$pd['projekt_id']." AND zeitart=102 AND mandanten_id=".$cid;
	}

	// Zeiten ändern
	if(sizeof($sarr))
	{
//fxDebug($sarr, '$sarr');
		foreach($sarr as $sql)
		{
			$rerr=db_x($sql);
//fxDebug($sql, '$rerr: '.$rerr);
			if($rerr)
				return $rerr;
		}
	}

	return false;
}

/**
 * Liefert bei Übergabe eines Startdatums und eines Aufwands (in Sekunden!) das Enddatum eines
 * Vorgangs (bereinigt die Dauer um Wochenenden und Feiertage)
 *
 * @param various $Start_Dtm - Mandatory parameter: ???PARAMETER???
 * @param various $aufw_sek  - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_ende($Start_Dtm, $aufw_sek)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$now=$Start_Dtm;
	while($aufw_sek > 0)
	{
		if(formatdatetime('w', $now) < 6)
		{
			$F_Tag=get_feiertage($now);
			if(($F_Tag[substr($now, 0, 8)]['art'] != FXP_HL_FULL) && ($F_Tag[substr($now, 0, 8)]['art'] != FXP_HL_HALF))
				$aufw_sek -= 3600*$GLOBALS['fxpglobals']['settings']['mtag'];
			if($F_Tag[substr($now, 0, 8)]['art'] == FXP_HL_HALF)
				$aufw_sek -= 1800*$GLOBALS['fxpglobals']['settings']['mtag'];
		}
		$now=time_add($now, 1, 'D');
		$days++;
	}
	if($days < 1)
		$days=1;
	$Ende_Dtm=time_add($Start_Dtm, $days-1, 'D');
	return $Ende_Dtm;
}

/**
 * Funktion setzt die Deadline bei den übergeordneten Projekten auf den höchsten Wert
 *
 * @param various $Projekt_ID - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function set_deadline($Projekt_ID)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!$Projekt_ID)
		return false;

	$superiors=superior_vn($Projekt_ID);
	if($superiors == '-666')
		return false;

	$rerr=false;
	$deadline_change=explode(',', $superiors);
	if(is_array($deadline_change))
	{
		foreach($deadline_change as $dc)
		{
			$spid=(int)$dc;
			if(!$spid)
				continue;

			// liefert höchste Deadline unter diesem Vorgang, vorausgesetz diese ist nach Soll-Ende dieses Vorgangs
			$Deadline=db_value("SELECT p.deadline FROM projekte p, zeitdaten z WHERE p.elter=".$spid." AND z.soll_ende<=p.deadline AND p.elter=z.projekt_id AND z.zeitart=102 ORDER BY p.deadline DESC");
			if($Deadline)
			{
				// Zusatz "and (select soll_ende from zeitdaten where projekt_id=".$deadline_change[$d]." and zeitart=102) < '".$Deadline."'"
				// war nötig, da sonst eine Aufgabe (1.1.2003 - 2.1.2003, Deadline 2.1.2003) die Deadline eines Über-Vorgangs (1.1.2003 - 11.1.2003)
				// auf den 1.2.2003 setzen konnte=unlogisch!
				$sql="UPDATE projekte SET deadline='".$Deadline."' WHERE projekt_id=".$spid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$rerr=db_x($sql);
				if($rerr)
					break;
			}
		}
	}

	return $rerr;
}

/**
 * ???FUNCTION???
 *
 * @param integer $elter - Optional parameter (default = 0):       ???PARAMETER???
 * @param integer $pid   - Optional parameter (default = 0):       ???PARAMETER???
 * @param array   $brt   - Optional parameter (default = array()): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_budgetrahmen($elter=0, $pid=0, $brt=array())
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$elter	= (int)$elter;
	$pid	= (int)$pid;
	$pie	= array('pers_int', 'pers_ext', 'mat_int', 'mat_ext');
	$pkt	= 'pers_int';
	$pkd	= 0;
	$cep	= $elter.'_'.$pid;

	if(isset($GLOBALS['_rem_budgetframe']) && is_array($GLOBALS['_rem_budgetframe']) && isset($GLOBALS['_rem_budgetframe']['id']) && ($GLOBALS['_rem_budgetframe']['id'] == $cep))
		return $GLOBALS['_rem_budgetframe'];

	$budgetrahmen=array
	(
		'id'=>$cep,

		'pers_int_elter'=>0.0,	'pers_int_max'=>0.0,	'pers_int_min'=>0.0,	'pers_int_rest'=>0.0,
		'pers_ext_elter'=>0.0,	'pers_ext_max'=>0.0,	'pers_ext_min'=>0.0,	'pers_ext_rest'=>0.0,

		'mat_int_elter'=>0.0,	'mat_int_max'=>0.0,	'mat_int_min'=>0.0,	'mat_int_rest'=>0.0,
		'mat_ext_elter'=>0.0,	'mat_ext_max'=>0.0,	'mat_ext_min'=>0.0,	'mat_ext_rest'=>0.0
	);


	if($elter || $pid)
	{
		$mnd=$GLOBALS['fxpglobals']['client'];
		$mid=0;
		$eid=0;
		$pvn='';
		$evn='';

		// Projektinfos ermitteln
		if(is_array($brt) && sizeof($brt) && is_array($brt[-1]))
		{
			$mid=$brt[-1]['mid'];
			$eid=$brt[-1]['eid'];
			$pvn=$brt[-1]['pvn'];
			$evn=$brt[-1]['evn'];

			unset($brt[-1]);
		}
		else if($pid)
		{
			$sql="SELECT mutterprojekt_id, elter, vorgangsnummer FROM projekte WHERE projekt_id=".$pid." AND mandanten_id=".$mnd;
			$tmp=db_values($sql);
//fxDebug($tmp, $sql);
			if(is_array($tmp))
			{
				$mid=(int)$tmp['mutterprojekt_id'];
				$eid=(int)$tmp['elter'];
				$pvn=trim($tmp['vorgangsnummer']);
				$evn=substr($pvn, 0, -5);
			}
		}
		else
		{
			$eid=$elter;
			$sql="SELECT mutterprojekt_id, vorgangsnummer FROM projekte WHERE projekt_id=".$eid." AND mandanten_id=".$mnd;
			$tmp=db_values($sql);
//fxDebug($tmp, $sql);
			if(is_array($tmp))
			{
				$mid=(int)$tmp['mutterprojekt_id'];
				$evn=trim($tmp['vorgangsnummer']);
			}
		}

		if($mid > 0)
		{
			if(!is_array($brt) || !sizeof($brt))
			{
				// Alle Projekte ermitteln
				$sel ="SELECT p.vorgangsnummer, b.projekt_id, b.projektart,";
				$sel .= " b.budgetrahmen_pers_int AS pers_int_ro, b.budgetrahmen_pers_ext AS pers_ext_ro, b.budgetrahmen_mat_int AS mat_int_ro, b.budgetrahmen_mat_ext AS mat_ext_ro,";
				$sel .= " b.budgetrahmen_pers_int AS pers_int_rahmen, b.budgetrahmen_pers_ext AS pers_ext_rahmen, b.budgetrahmen_mat_int AS mat_int_rahmen, b.budgetrahmen_mat_ext AS mat_ext_rahmen,";
				$sel .= " b.budget_pers_int_gepl AS pers_int_gepl, b.budget_pers_ext_gepl AS pers_ext_gepl, b.budget_mat_int_gepl AS mat_int_gepl, b.budget_mat_ext_gepl AS mat_ext_gepl,";
				$sel .= " -1.00 AS pers_int_max, -1.00 AS pers_ext_max, -1.00 AS mat_int_max, -1.000 AS mat_ext_max,";
				$sel .= " b.budget_pers_int_gepl AS pers_int_min, b.budget_pers_ext_gepl AS pers_ext_min, b.budget_mat_int_gepl AS mat_int_min, b.budget_mat_ext_gepl AS mat_ext_min";
				$sel .= " FROM budget_summe b, projekte p";
				$sel .= " WHERE p.mutterprojekt_id=".$mid." AND p.mandanten_id=".$mnd." AND p.projekt_id=b.projekt_id AND b.mandanten_id=".$mnd;

				$sql =$sel." AND ";
				if($pid)
					$sql .= "(p.projekt_id=".$pid." OR ";
				$sql .="p.projektart<".FXP_PRJ_TASK;
				if($pid)
					$sql .= ")";
				$sql .= " ORDER BY p.vorgangsnummer";
				$brt =db_values($sql, '*');
			}

			if(is_array($brt) && sizeof($brt))
			{
				$bri=array();
				$vns=array();

				foreach($brt as $brc => $b)
				{
					$id=(int)$b['projekt_id'];

					$bri[$id]=$brc;
					$vns[$b['vorgangsnummer']]=$id;

					// Rundungsfehler vorbeugen, deshalb Budgetfelder auf 4 Stellen hinter dem Komma runden
					foreach($b as $bf => $bv)
					{
						$bf3=substr($bf,0,3);
						if(($bf3 == 'per') || ($bf3 == 'mat'))
							$brt[$brc][$bf]=round(get_double($bv),4);
					}
				}
//fxDebug($brt, $sql);

				$vnr=$vns;
				krsort($vnr);

//fxDebug($bri, '$bri');
//fxDebug($vns, '$vns');
//fxDebug($vnr, '$vnr');

				// Rahmen
if(strlen($pkt) && ($pkd > 1))
echo('<hr /><b><u>Rahmen festlegen:</u></b><br />'.$nl);
				foreach($vns as $vn => $id)
				{
if(strlen($pkt) && ($pkd > 1))
echo('<b>['.$vn.']:</b> start &quot;'.$pkt.'&quot; -- ro='.$brt[$bri[$id]][$pkt.'_ro'].', rahmen='.$brt[$bri[$id]][$pkt.'_rahmen'].' ...<br />'.$nl);
					$b=$brt[$bri[$id]];
					foreach($pie as $pkey)
					{
						if($b[$pkey.'_ro'] > 0.0)
						{
							$cvn=$vn.'.';
							$cvl=strlen($cvn);
							foreach($vnr as $rvn => $rid)
							{
								if((substr($rvn, 0, $cvl) === $cvn) && ($brt[$bri[$rid]][$pkey.'_ro'] == 0.0))
								{
									$brt[$bri[$rid]][$pkey.'_rahmen']=$b[$pkey.'_ro'];
if(strlen($pkt) && ($pkey == $pkt) && ($pkd > 1))
echo('&nbsp;&nbsp;&nbsp;-&gt; ['.$rvn.'] &quot;'.$pkt.'&quot; set to '.$b[$pkey.'_ro'].' !<br />'.$nl);
								}
							}
						}
					}
				}
				if(strlen($pkt) && $pkd)
				{
					echo('<hr /><b><u>Rahmen &Uuml;bersicht:</u></b><br />'.$nl);
					foreach($vns as $vn => $id)
						echo('<b>['.$vn.']:</b> &quot;'.$pkt.'&quot;='.get_entry($brt[$bri[$id]][$pkt.'_rahmen'], 'dezimal').'<br />'.$nl);
				}

				// Minimum
if(strlen($pkt) && ($pkd > 1))
echo('<hr /><b><u>Minimum festlegen:</u></b><br />'.$nl);
				foreach($vnr as $vn => $id)
				{
if(strlen($pkt) && ($pkd > 1))
echo('<b>['.$vn.']:</b> start &quot;'.$pkt.'&quot; -- gepl='.$brt[$bri[$id]][$pkt.'_gepl'].', min='.$brt[$bri[$id]][$pkt.'_min'].' ...<br />'.$nl);
					$b=$brt[$bri[$id]];
					foreach($pie as $pkey)
					{
						if($pid && ($id == $pid) && ($brt[$bri[$id]]['projektart'] == FXP_PRJ_TASK))
							$brt[$bri[$id]][$pkey.'_min']=0.0;
						else if($b[$pkey.'_ro'] > 0.0)
						{
							$cvn=substr($vn, 0, -5);
							while(strlen($cvn) > 3)
							{
								$nid=$vns[$cvn];
if(strlen($pkt) && ($pkey == $pkt) && ($pkd > 1))
echo('&nbsp;&nbsp;&nbsp;-&gt; ['.$cvn.'] &quot;'.$pkt.'&quot; '.$brt[$bri[$nid]][$pkey.'_min'].' += '.($b[$pkey.'_ro']-$b[$pkey.'_min']));
								$brt[$bri[$nid]][$pkey.'_min'] += ($b[$pkey.'_ro']-$b[$pkey.'_min']);
if(strlen($pkt) && ($pkey == $pkt) && ($pkd > 1))
echo(' =&gt; '.$brt[$bri[$nid]][$pkey.'_min'].' !<br />'.$nl);

								$cvn=substr($cvn, 0, -5);
							}
						}
					}
				}
				if(strlen($pkt) && $pkd)
				{
					echo('<hr /><b><u>Minimum &Uuml;bersicht:</u></b><br />'.$nl);
					foreach($vns as $vn => $id)
						echo('<b>['.$vn.']:</b> &quot;'.$pkt.'&quot;='.$brt[$bri[$id]][$pkt.'_min'].'<br />'.$nl);
				}

				// Maximum
if(strlen($pkt) && ($pkd > 1))
echo('<hr /><b><u>Maximum festlegen:</u></b><br />'.$nl);
				foreach($vns as $vn => $id)
				{
if(strlen($pkt) && ($pkd > 1))
echo('<b>['.$vn.']:</b> start &quot;'.$pkt.'&quot; -- rahmen='.$brt[$bri[$id]][$pkt.'_rahmen'].', min='.$brt[$bri[$id]][$pkt.'_min'].', max='.$brt[$bri[$id]][$pkt.'_max'].' ...<br />'.$nl);
					if(strlen($vn) == 4)
						continue;

					// Elter
					$cvn=substr($vn, 0, -5);
					$b=$brt[$bri[$vns[$cvn]]];
					foreach($pie as $pkey)
					{
if(strlen($pkt) && ($pkey == $pkt) && ($pkd > 1))
echo('&nbsp;&nbsp;&nbsp;-&gt; Elter ['.$b['vorgangsnummer'].'] &quot;'.$pkt.'&quot; -- rahmen='.$b[$pkey.'_rahmen'].', max='.$b[$pkey.'_max'].'<br />'.$nl);
						if(($b[$pkey.'_max'] > 0.0) && ($b[$pkey.'_rahmen'] > 0.0))
						{
							$bmr=min($b[$pkey.'_max'], $b[$pkey.'_rahmen']);
if(strlen($pkt) && ($pkey == $pkt) && ($pkd > 1))
echo('&nbsp;&nbsp;&nbsp;-&gt; ['.$vn.'] &quot;'.$pkt.'&quot; -- Mimumum der beiden='.$bmr.' !<br />'.$nl);
						}
						else if($b[$pkey.'_max'] > 0.0)
						{
							$bmr=$b[$pkey.'_max'];
if(strlen($pkt) && ($pkey == $pkt) && ($pkd > 1))
echo('&nbsp;&nbsp;&nbsp;-&gt; ['.$vn.'] &quot;'.$pkt.'&quot; -- Max='.$bmr.' !<br />'.$nl);
						}
						else if($b[$pkey.'_rahmen'] > 0.0)
						{
							$bmr=$b[$pkey.'_rahmen'];
if(strlen($pkt) && ($pkey == $pkt) && ($pkd > 1))
echo('&nbsp;&nbsp;&nbsp;-&gt; ['.$vn.'] &quot;'.$pkt.'&quot; -- Rahmen='.$bmr.' !<br />'.$nl);
						}
						else
							$bmr=0.0;

						if($bmr > 0.0)
						{
							$brt[$bri[$id]][$pkey.'_max']=$bmr-($b[$pkey.'_gepl']-$brt[$bri[$id]][$pkey.'_gepl']);
if(strlen($pkt) && ($pkey == $pkt) && ($pkd > 1))
echo('&nbsp;&nbsp;&nbsp;-&gt; ['.$vn.'] &quot;'.$pkt.'&quot; -- abzgl. '.($b[$pkey.'_gepl']-$brt[$bri[$id]][$pkey.'_gepl']).'='.$brt[$bri[$id]][$pkey.'_max'].' !<br />'.$nl);

							$cvn .= '.';
							$cvl=strlen($cvn);
							foreach($vnr as $rvn => $rid)
							{
								if(($rid != $id) && (substr($rvn, 0, $cvl) === $cvn) && (strlen($rvn) == $cvl+4))
								{
									if($brt[$bri[$rid]][$pkey.'_ro'] > 0.0)
									{
										$bd=($brt[$bri[$rid]][$pkey.'_ro']-$brt[$bri[$rid]][$pkey.'_gepl']);
if(strlen($pkt) && ($pkey == $pkt) && ($pkd > 1))
echo('&nbsp;&nbsp;&nbsp;-&gt; ['.$cvn.'] &quot;'.$pkt.'&quot; (ro) -- '.$brt[$bri[$id]][$pkey.'_max'].' -= '.$bd);
									}
									else
									{
										$bd=($brt[$bri[$rid]][$pkey.'_min']-$brt[$bri[$rid]][$pkey.'_gepl']);
if(strlen($pkt) && ($pkey == $pkt) && ($pkd > 1))
echo('&nbsp;&nbsp;&nbsp;-&gt; ['.$cvn.'] &quot;'.$pkt.'&quot; (min) -- '.$brt[$bri[$id]][$pkey.'_max'].' -= '.$bd);
									}
									$brt[$bri[$id]][$pkey.'_max']=round($brt[$bri[$id]][$pkey.'_max'] - $bd, 4);
if(strlen($pkt) && ($pkey == $pkt) && ($pkd > 1))
echo(' =&gt; '.$brt[$bri[$id]][$pkey.'_max'].' !<br />'.$nl);
								}
							}
						}
					}
				}
				if(strlen($pkt) && $pkd)
				{
					echo('<hr /><b><u>Maximum &Uuml;bersicht:</u></b><br />'.$nl);
					foreach($vns as $vn => $id)
						echo('-&gt; <b>['.$vn.']:</b> &quot;'.$pkt.'&quot;='.$brt[$bri[$id]][$pkt.'_max'].'<br />'.$nl);
				}

				// Ermittelte Werte zuweisen
				$id=$pid;
				if(!isset($bri[$id]))
					$id=$eid;
				foreach($pie as $pkey)
				{
					if(strlen($evn))
						$budgetrahmen[$pkey.'_elter']=get_double($brt[$bri[$eid]][$pkey.'_rahmen']);

					if($id == $pid)
					{
						$budgetrahmen[$pkey.'_min']=get_double($brt[$bri[$id]][$pkey.'_min']);
						$budgetrahmen[$pkey.'_max']=get_double($brt[$bri[$id]][$pkey.'_max']);
					}
					else
					{
						if(($budgetrahmen[$pkey.'_elter'] > 0.0) && ($brt[$bri[$id]][$pkey.'_max'] >= 0.0))
							$budgetrahmen[$pkey.'_max']=min(get_double($brt[$bri[$id]][$pkey.'_rahmen']), get_double($brt[$bri[$id]][$pkey.'_max']));
						else
							$budgetrahmen[$pkey.'_max']=get_double($brt[$bri[$id]][$pkey.'_rahmen']);
						$budgetrahmen[$pkey.'_max']=round($budgetrahmen[$pkey.'_max'] - $brt[$bri[$id]][$pkey.'_min'], 4);
					}

					if($budgetrahmen[$pkey.'_max'] <= 0.0)
						$budgetrahmen[$pkey.'_max']=0.0;

					$budgetrahmen[$pkey.'_rest']=$budgetrahmen[$pkey.'_max'];
				}
			}
		}
	}

	if($pkd)
		fxDebug($budgetrahmen, '$budgetrahmen');

	$GLOBALS['_rem_budgetframe']=$budgetrahmen;
	return $budgetrahmen;
}

/**
 * $mode: 0=Funktion update_budget, 1=Vorgänge (Detail), 2=Vorgänge (Matrix)
 *
 * @param various $mode         - Mandatory parameter: ???PARAMETER???
 * @param various $budgetrahmen - Mandatory parameter: ???PARAMETER???
 * @param various $pdat         - Mandatory parameter: ???PARAMETER???
 * @param various $iea          - Mandatory parameter: ???PARAMETER???
 * @param various $pma          - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function validate_budgetrahmen($mode, $budgetrahmen, $pdat, $iea, $pma)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mode=(int)$mode;
	if(($mode < 0) || ($mode > 2))
		return false;

	if(!is_array($budgetrahmen) || !sizeof($budgetrahmen))
		return false;
	if(!is_array($pdat) || !sizeof($pdat))
		return false;
	if(!is_array($iea) || !sizeof($iea))
		return false;
	if(!is_array($pma) || !sizeof($pma))
		return false;

	$rerr		= array();
	$mwaehrung	= ' '.$GLOBALS['fxpglobals']['settings']['waehrung'];

	if($mode != 1)
		$pdat['aktion']=FXP_CHANGE;
//fxDebug($budgetrahmen, '$budgetrahmen');
//fxDebug($pdat, '$mode='.$mode);

	// Internes und externes Budget prüfen, ob es in den Rahmen passt (nur wenn Vorgang Elter hat)
	$rcode=0;
	if($pdat['elter'])
	{
		foreach($iea as $ie)
		{
//echo('$ie='.$ie.'<br />');
			// Personen- und Materialbudget prüfen
			foreach($pma as $pm)
			{
//echo('. $pm='.$pm.'<br />');
				$budgetrahmen_max	= $budgetrahmen[$pm.'_'.$ie.'_max'];
				$budgetrahmen_elter	= $budgetrahmen[$pm.'_'.$ie.'_elter'];

				// Neuer Budgetwert abhängig von Projektart
				if($pdat['projektart'] == FXP_PRJ_TASK)
					$field='budget_'.$pm.'_'.$ie.'_gepl';
				else
					$field='budgetrahmen_'.$pm.'_'.$ie;
				$new_budget=get_double($pdat[$field]);

				// Kein neues Budget oder kein Rahmen festgelegt, dann nächste Budgetart prüfen
				if(($new_budget == 0.0) || ($budgetrahmen_elter <= 0.0))
					continue;

				// Prüfung, ob Maximalwert überschritten
				if($new_budget > $budgetrahmen_max)
				{
//echo('.. $new_budget='.$new_budget.' > $budgetrahmen_max='.$budgetrahmen_max.'<br />');
					$diff_budget=$new_budget-$budgetrahmen_max;

					// 675: Der $var1 Budgetrahmen wurde um $var2 überschritten! Ihnen stehen nur maximal $var3 zur Verfügung - Sie haben aber $var4 geplant. Bitte verkleinern Sie den neuen Planwert oder erhöhen Sie den Budgetrahmen.
					if($mode == 1)
						$rerr[]="error_msg(675, array('budgetrahmen_".$pm."_".$ie."', '".$field."'), '".$ie."', '".get_entry($diff_budget, 'betrag').$mwaehrung."', '".get_entry($budgetrahmen_max, 'betrag').$mwaehrung."', '".get_entry($new_budget, 'betrag').$mwaehrung."');";
					else if($mode == 2)
						$rerr[$field][]=array(675, array('budgetrahmen_'.$pm.'_'.$ie, $field), $ie, get_entry($diff_budget, 'betrag').$mwaehrung, get_entry($budgetrahmen_max, 'betrag').$mwaehrung, get_entry($new_budget, 'betrag').$mwaehrung);
					else if($ie == 'int')
					{
						if($pdat['projektart'] == FXP_PRJ_TASK)
							$rcode=1229;	// Ihr int. geplantes Personal- oder Materialbudget überschreitet den festgelegten Budgetrahmen des übergeordneten Projektes! Bitte setzen Sie sich mit dem Projektleiter in Verbindung oder erhöhen Sie den Rahmen.
						else
							$rcode=1236;	// Ihr int. Personal- oder Materialbudgetrahmen überschreitet den festgelegten Budgetrahmen des übergeordneten Projektes! Bitte setzen Sie sich mit dem Projektleiter in Verbindung oder erhöhen Sie den Rahmen.
					}
					else if($pdat['projektart'] == FXP_PRJ_TASK)
						$rcode=1457;		// Ihr ext. geplantes Personal- oder Materialbudget überschreitet den festgelegten Budgetrahmen des übergeordneten Projektes! Bitte setzen Sie sich mit dem Projektleiter in Verbindung oder erhöhen Sie den Rahmen.
					else
						$rcode=1527;		// Ihr ext. Personal- oder Materialbudgetrahmen überschreitet den festgelegten Budgetrahmen des übergeordneten Projektes! Bitte setzen Sie sich mit dem Projektleiter in Verbindung oder erhöhen Sie den Rahmen.
				}
				if($rcode > 0)
					break;
			}
			if($rcode > 0)
				break;
		}
	}

	if($rcode > 0)
		return $rcode;

	// Nur für (Teil)-Projekte relevant -> geplanter Budgetrahmen dieses Projektes kleiner als Budgetsumme der Untervorgänge
	if(($pdat['projektart'] < FXP_PRJ_TASK) && ($pdat['aktion'] == FXP_CHANGE))
	{
		foreach($iea as $ie)
		{
			// Personen- und Materialbudget prüfen
			foreach($pma as $pm)
			{
				$field				= 'budgetrahmen_'.$pm.'_'.$ie;
				$budgetrahmen_max	= $budgetrahmen[$pm.'_'.$ie.'_max'];
				$budgetrahmen_min	= $budgetrahmen[$pm.'_'.$ie.'_min'];
				$new_budgetrahmen	= get_double($pdat[$field]);		// Wert des internen Budgetrahmens
				if(($new_budgetrahmen != 0.0) && ($new_budgetrahmen < $budgetrahmen_min))
				{
					$diff_budget=$budgetrahmen_min-$new_budgetrahmen;

					if($budgetrahmen_max == 0.0)
						$var4='&infin;';
					else
						$var4=get_entry(max($budgetrahmen_max, $budgetrahmen_min), 'betrag');

					// 676: Der $var1 Budgetrahmen wurde um $var2 unterschritten, da bereits für nachfolgende Vorgänge $var3 geplant wurden! Der neue Wert muss zwischen $var3 und $var4 sein.
					if($mode == 1)
						$rerr[]="error_msg(676, array('".$field."'), '".$ie."', '".get_entry($diff_budget, 'betrag').$mwaehrung."', '".get_entry($budgetrahmen_min, 'betrag').$mwaehrung."', '".$var4.$mwaehrung."');";
					else if($mode == 2)
						$rerr[$field][]=array(676, array($field), $ie, get_entry($diff_budget, 'betrag').$mwaehrung, get_entry($budgetrahmen_min, 'betrag').$mwaehrung, $var4.$mwaehrung);
					else if($ie == 'int')
						$rcode=1487;	// Ihr int. Personal- oder Materialbudgetrahmen unterschreitet die bereits für nachfolgende Vorgänge geplante Summe! Bitte setzen Sie sich mit dem Projektleiter in Verbindung oder erhöhen Sie den Rahmen.
					else
						$rcode=1488;	// Ihr ext. Personal- oder Materialbudgetrahmen unterschreitet die bereits für nachfolgende Vorgänge geplante Summe! Bitte setzen Sie sich mit dem Projektleiter in Verbindung oder erhöhen Sie den Rahmen.
				}
				if($rcode > 0)
					break;
			}
			if($rcode > 0)
				break;
		}
	}

	if($rcode > 0)
		return $rcode;

	if(!$mode)
		return false;
	return $rerr;
}

/**
 * Funktion handelt sämtliche Budget- und Rechnungsdaten zu einem Projekt
 * -----------------------------------------------------------------------------
 * HINWEIS:
 * Übergebene Werte entsprechen immer der Gesamtsumme, ausser den Werten wurde
 * ein + oder - vorangestellt, dann sind es Deltawerte.
 *
 * @param various $pid             - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $barr            - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $change_aendzeit - Optional parameter (default = true):   ???PARAMETER???
 * @param boolean $make_history    - Optional parameter (default = true):   ???PARAMETER???
 *
 * @return ???RETURN???
 */
function update_budget($pid, $barr=false, $change_aendzeit=true, $make_history=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pid=(int)$pid;
	if(!$pid)
		return 1805;	// Es wurde keine Projekt-ID übermittelt oder die Projekt-ID ist 0!

//$_GET['test']='1969';
	if($_GET['test'] == '1969')
	{
		$GLOBALS['force_fxDebug']=true;
		fxDebug($barr,$pid);
	}

	$mid	= $GLOBALS['fxpglobals']['client'];
	$vne	= '';
	$vex	= false;
	$fbud	= false;	// Force Budget, d.h. Schreiben eines Budgetsatzes erzwingen
	$fres	= false;	// Force Ressource, d.h. Schreiben eines Ressourcensatzes erzwingen

	$budgetgen=false;
	if(strtoupper(substr($GLOBALS['fxpglobals']['settings']['budgetgen'], 0, 1)) == 'N')
		$budgetgen=true;

	$budgetgenaktiv=false;
	if(strtoupper(substr($GLOBALS['fxpglobals']['settings']['budgetgenaktiv'], 0, 1)) == 'J')
		$budgetgenaktiv=true;

	// Prüfen, ob es das Projekt bereits gibt
	$sql="SELECT mandanten_id FROM projekte WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
	$cpe=(int)db_value($sql);
	if($cpe)
		$vex=true;

	// Wurde explizit mitgegeben in der Matrix dass Projekt gelöscht wurde
	if($barr === 'deleted')
	{
		$vex=false;
		$barr=false;
	}

	// Im Neuanlage- oder Änderungsmodus müssen Werte mitgegeben werden
	if(!is_array($barr) && $vex)
		return 674;		// Es wurde kein Argumente-Array übermittelt oder das Array ist leer!

	// Mutterprojekt-ID, Elter, Vorgangsnummer, Projektart, Projektstatus und Soll-Aufwand zu diesem Projekt ermitteln
	$sql="SELECT autoid, mutterprojekt_id, elter, vorgangsnummer, projektart, projekt_status, aufwand_soll, kategorie_id FROM projekte_history WHERE projekt_id=".$pid." AND mandanten_id=".$mid." ORDER BY autoid DESC";
	$prj=db_values($sql);
	if(is_array($prj))
	{
//fxDebug($prj, $sql);
		$aid=(int)$prj['autoid'];
		$mpi=(int)$prj['mutterprojekt_id'];
		$eid=(int)$prj['elter'];
		$vn =trim($prj['vorgangsnummer']);
		$pra=(int)$prj['projektart'];
		$pst=(int)$prj['projekt_status'];
		$aws=get_double($prj['aufwand_soll']);
		$pki=(int)$prj['kategorie_id'];
	}
	else
	{
		$aid=-1;
		$mpi=$pid;
		$eid=0;
		$vn ='';
		$pra=FXP_PRJ_TASK;
		$pst=FXP_PS_PLANNED;
		$aws=0.0;
		$pki=0;
	}

	// Untergeordnete Vorgänge
	if($pra >= FXP_PRJ_TASK)
		$vinf=$pid;
	else
		$vinf=inferior_vn($pid);
//fxDebug($vinf, '$vinf');

	// Alten Projektstatus ermitteln
	$old_pst=0;
	$old_pra=0;
	if($aid > 0)
	{
		$sql="SELECT projekt_status, projektart FROM projekte_history WHERE projekt_id=".$pid." AND mandanten_id=".$mid." AND autoid<".$aid." ORDER BY autoid DESC";
		$old_pp=db_values($sql);
		if(is_array($old_pp))
		{
			$old_pst=(int)$old_pp['projekt_status'];
			$old_pra=(int)$old_pp['projektart'];
		}
	}
	if(!$old_pst)
		$old_pst=$pst;
	if(!$old_pra)
		$old_pra=$pra;

	// Budgetdatensatz forcieren, falls alter Status=gepl. oder inaktiv und neuer Status=aktiv oder abgeschl.
	if(($pst != $old_pst) && ($old_pst < FXP_PS_ACTIVE) && ($pst > FXP_PS_INACTIVE))
		$fbud=true;
//fxDebug($fbud, '<B>Budget:</B> '.$old_pst.' -&gt; '.$pst);
	// Budgetdatensatz forcieren, falls Aufgabe und alter Status=gepl. oder inaktiv und neuer Status=aktiv oder abgeschl.
	if(($pra >= FXP_PRJ_TASK) && ($pst != $old_pst) && ($old_pst < FXP_PS_ACTIVE) && ($pst > FXP_PS_INACTIVE))
		$fres=true;
//fxDebug($fres, '<B>Ressource:</B> '.$old_pst.' -&gt; '.$pst);

	// Parameter typ: Bit 0=1 (Startvorgang), Bit 1=2 (Eltervorgang des neuen Pfades), Bit 2=4 (Eltervorgang des alten Pfades)
	$path=array($pid => array('typ'=>1, 'old'=>array(), 'new'=>array()));

	// Aktuellen Pfad nach oben ermitteln
	$ebk=array($pid=>true);
	$elt=$eid;
	while($elt)
	{
		$sql="SELECT elter, vorgangsnummer FROM projekte_history WHERE projekt_id=".$elt." AND mandanten_id=".$mid." ORDER BY autoid DESC";
		$prj=db_values($sql);
//fxDebug($prj, $sql);

		if(is_array($prj))
		{
			$ebk[$elt]=true;

			if(!is_array($path[$elt]))
				$path[$elt]=array('typ'=>2, 'old'=>array(), 'new'=>array());
			else
				$path[$elt]['typ'] |= 2;

			$elt=(int)$prj['elter'];
			if($ebk[$elt])
				break;
		}
		else
			break;
	}
	// Alten Pfad nach oben ermitteln
	if($aid > 0)
	{
		$ebk=array($pid=>true);
		$elt=$pid;
		while($elt)
		{
			$sql="SELECT autoid, elter, vorgangsnummer FROM projekte_history WHERE projekt_id=".$elt." AND mandanten_id=".$mid." AND autoid<".$aid." ORDER BY autoid DESC";
			$prj=db_values($sql);
//fxDebug($prj, $sql);

			if(is_array($prj))
			{
				if($elt != $pid)
				{
					$ebk[$elt]=true;

					if(!is_array($path[$elt]))
						$path[$elt]=array('typ'=>4, 'old'=>array(), 'new'=>array());
					else
						$path[$elt]['typ'] |= 4;
				}

				$aid=(int)$prj['autoid'];
				$elt=(int)$prj['elter'];
				if($ebk[$elt])
					break;
			}
			else
			{
				foreach($path as $tpid => $p)
				{
					if($tpid != $pid)
						$path[$tpid]['typ'] |= 4;
				}
				break;
			}
		}
	}
//fxDebug($path, __LINE__.': $path');

	// Budgetbestandsdaten ermitteln
	$pids_csv=array_to_csv($path);
//echo('$pids_csv='.$pids_csv.'<br />'.$nl);
	$sql="SELECT * FROM budget_summe WHERE projekt_id IN (".$pids_csv.") AND mandanten_id=".$mid;
	$akt=db_values($sql, '*');
//fxDebug($akt, $sql);
	if(is_array($akt))
	{
		foreach($akt as $a)
		{
			$tpid=(int)$a['projekt_id'];

			if($tpid == $pid)
			{
				$sqlz="SELECT gen_beginn, gen_ende FROM zeitdaten WHERE projekt_id=".$tpid." AND zeitart=102 AND mandanten_id=".$mid;
				$gen=db_values($sqlz);
				if(!is_array($gen))
					$gen=array('gen_beginn'=>"", 'gen_ende'=>"");
				$a['gen_beginn']	= $gen['gen_beginn'];
				$a['gen_ende']		= $gen['gen_ende'];

				if(!strlen($a['budgettyp']))
				{
					$sqlz="SELECT budgettyp FROM budget_detail WHERE projekt_id=".$tpid." AND mandanten_id=".$mid." ORDER BY zeitstempel DESC";
					$a['budgettyp']=(int)db_value($sqlz);
				}
			}

			$path[$tpid]['old']=$a;
			$path[$tpid]['new']=$a;
		}
	}
//fxDebug($path, __LINE__.': $path');

	$old=$path[$pid]['old'];
//fxDebug($old, '$old');

	// Bestehende Budgetdaten des aktuellen Vorgangs im alten Pfad abziehen
	if(sizeof($old))
	{
		foreach($path as $tpid => $p)
		{
			// Ausnahmen: Startvorgang, Nicht im "alten" Pfad, Keine vorhandenen Budgetdaten (da z.B. bereits gelöscht)
			if(($tpid == $pid) || !($p['typ']&4) || !sizeof($p['new']))
				continue;

			foreach($p['new'] as $field => $value)
			{
				if((substr($field, 0, 7) == 'budget_') || (substr($field, 0, 9) == 'rechnung_'))
					$path[$tpid]['new'][$field]=$path[$tpid]['new'][$field]-$old[$field];
			}
		}
	}
//fxDebug($path, __LINE__.': $path');

	// Elter-Budgetrahmen (nur, falls nicht Vorgänge (Detail/Matrix), da dort bereits geprüft)
	if(($GLOBALS['fxpglobals']['tr'] != 23) && ($GLOBALS['fxpglobals']['tr'] != 189))
		$budgetrahmen=get_budgetrahmen($eid, $pid);
	else
		$budgetrahmen=false;

	// Typarrayy
	$pma=array('pers', 'mat');
	$iea=array('int', 'ext');

	// Budgetarrays festlegen
	$bf_rahmen=array
	(
		'budgetrahmen_pers_int',	'budgetrahmen_pers_ext',	'budgetrahmen_mat_int',		'budgetrahmen_mat_ext'
	);
	$bf_plan=array
	(
		'budget_pers_int_gepl',		'budget_pers_ext_gepl',		'budget_mat_int_gepl',		'budget_mat_ext_gepl',
		'budget_pers_int_gen',		'budget_pers_ext_gen',		'budget_mat_int_gen',		'budget_mat_ext_gen'
	);
	$bf_kalkuliert=array
	(
		'budget_pers_int_akt',		'budget_pers_ext_akt',		'budget_mat_int_akt',		'budget_mat_ext_akt',
		'budget_pers_int_verpl',	'budget_pers_ext_verpl',	'budget_mat_int_verpl',		'budget_mat_ext_verpl',
		'budget_pers_int_verbr',	'budget_pers_ext_verbr',	'budget_mat_int_verbr',		'budget_mat_ext_verbr',
		'budget_pers_ext_abger',	'budget_mat_ext_abger'
	);
	$bf_rechnung=array
	(
		'rechnung_fest_ext',		'rechnung_std_ext',		'rechnung_reise_ext',		'rechnung_mat_ext',
		'rechnung_fest_ext_off',	'rechnung_std_ext_off',	'rechnung_reise_ext_off',	'rechnung_mat_ext_off'
	);

	// Übergabewerte ermitteln
	$detail_change=false;
	// ...für Budgetrahmen und Budgetplanung
	$bfarr=array('Budgetrahmen'=>$bf_rahmen, 'Budgetplanung'=>$bf_plan);
	foreach($bfarr as $bn => $bfsubarr)
	{
		$lbn=substr($bn, -1);
		foreach($bfsubarr as $feldname)
		{
			// 0, falls [Vorgang nicht (mehr) existiert]
			if(!$vex)
				${$feldname}=0.0;
			// falls Projektart gleich und [Budgetrahmen und Aufgaben] oder [Budgetplanung und Projekt]
			else if(($pra == $old_pra) && ((($lbn == 'n') && ($pra >= FXP_PRJ_TASK)) || (($lbn != 'n') && ($pra < FXP_PRJ_TASK))))
			{
				if(sizeof($old))
					${$feldname}=get_double($old[$feldname]);
				else
					${$feldname}=0.0;
			}
			// Neuer Wert, falls übermittelt
			else if(isset($barr[$feldname]))
			{
				${$feldname}=get_double($barr[$feldname]);
				$vz=substr($barr[$feldname], 0, 1);
				if(($vz == '+') || ($vz == '-'))
				{
					if(sizeof($old))	// Differenzbetrag
						${$feldname}=(double)(${$feldname} + get_double($old[$feldname]));
					else if($vz == '-')	// Keine neg. Werte
						${$feldname}=0.0;
					else				// Nur. pos. Werte
						${$feldname}=(double)substr(${$feldname}, 1);
				}
			}
			// Alter Wert, falls existiert
			else if(sizeof($old))
				${$feldname}=get_double($old[$feldname]);
			// 0, sonst
			else
				${$feldname}=0.0;

			// Eintrag in "budget_detail" muss erstellt werden, falls [kein alter Wert] oder [neuer Wert ungleich alter Wert]
			if(!sizeof($old) || (${$feldname} != get_double($old[$feldname])))
				$detail_change=true;
//echo($bn.': $old['.$feldname.']='.$old[$feldname].' -- <B>$'.$feldname.'</B>='.${$feldname}.'<br />'.$GLOBALS['nl']);
		}
//echo('<hr />'.$GLOBALS['nl']);
	}
	// ...für Budgetkalkulation und Budgetrechnung
	$bfarr=array('Budgetkalkulation'=>$bf_kalkuliert, 'Budgetrechnung'=>$bf_rechnung);
	foreach($bfarr as $bn => $bfsubarr)
	{
		foreach($bfsubarr as $feldname)
		{
			if(sizeof($old))
				${$feldname}=get_double($old[$feldname]);
			else
				${$feldname}=0.0;
//echo($bn.': $old['.$feldname.']='.$old[$feldname].' -- <B>$'.$feldname.'</B>='.${$feldname}.'<br />'.$GLOBALS['nl']);
		}
//echo('<hr />'.$GLOBALS['nl']);
	}

	// Abrechnungsart (Personen)
	$abrechnungsart=(int)$barr['abrechnungsart'];
	if(!$abrechnungsart)
	{
		if(sizeof($old))
			$abrechnungsart=(int)$old['abrechnungsart'];
		if(!$abrechnungsart)
		{
			$abrechnungsart=$GLOBALS['fxpglobals']['settings']['def_abrechnart'];
			if(!$abrechnungsart || ($abrechnungsart == FXP_INV_MAT_HU))
				$abrechnungsart=FXP_INV_HOUR;	// Stundenbasis
		}
	}

	// Abrechnungsart (Material)
	$abrechnungsart_mat=(int)$barr['abrechnungsart_mat'];
	if(!$abrechnungsart_mat)
	{
		if(sizeof($old))
			$abrechnungsart_mat=(int)$old['abrechnungsart_mat'];
		if(!$abrechnungsart_mat)
		{
			$abrechnungsart_mat=$GLOBALS['fxpglobals']['settings']['def_abrechnart'];
			if(!$abrechnungsart_mat || ($abrechnungsart_mat == FXP_INV_HOUR))
				$abrechnungsart=FXP_INV_MAT_HU;	// Mengenbasis
		}
	}

	// Budgettyp
	$budgettyp=(int)$barr['budgettyp'];
	if(!$budgettyp)
	{
		if(sizeof($old))
			$budgettyp=(int)$old['budgettyp'];
		if(!$budgettyp)
			$budgettyp=702;	// Standardbudget
	}
	if(!sizeof($old) || ($budgettyp != $old['budgettyp']))
		$detail_change=true;

	// Gen_Beginn
	$gen_beginn=trim($barr['gen_beginn']);
	if(strlen($gen_beginn))
	{
		if($gen_beginn == 'NULL')
			$gen_beginn='';
		if(!sizeof($old) || ($old['gen_beginn'] != $gen_beginn))
			$detail_change=true;
		if(strlen($gen_beginn))
			$gen_beginn="'".$gen_beginn."'";
		else
			$gen_beginn='NULL';
	}
	else if(sizeof($old) && strlen($old['gen_beginn']))
		$gen_beginn="'".$old['gen_beginn']."'";
	else
		$gen_beginn='NULL';

	// Gen_Ende
	$gen_ende=trim($barr['gen_ende']);
	if(strlen($gen_ende))
	{
		if($gen_ende == 'NULL')
			$gen_ende='';
		if(!sizeof($old) || ($old['gen_ende'] != $gen_ende))
			$detail_change=true;
		if(strlen($gen_ende))
			$gen_ende="'".$gen_ende."'";
		else
			$gen_ende='NULL';
	}
	else if(sizeof($old) && strlen($old['gen_ende']))
		$gen_ende="'".$old['gen_ende']."'";
	else
		$gen_ende='NULL';

	// Budgets prüfen, ob diese im Rahmen liegen etc.
	if(is_array($budgetrahmen))
	{
		$pdat=array(
			'elter'			=> $eid,
			'projektart'	=> $pra,

			'budgetrahmen_pers_int'	=>	$budgetrahmen_pers_int,
			'budgetrahmen_pers_ext'	=>	$budgetrahmen_pers_ext,
			'budgetrahmen_mat_int'	=>	$budgetrahmen_mat_int,
			'budgetrahmen_mat_ext'	=>	$budgetrahmen_mat_ext,

			'budget_pers_int_gepl'	=>	$budget_pers_int_gepl,
			'budget_pers_ext_gepl'	=>	$budget_pers_ext_gepl,
			'budget_mat_int_gepl'	=>	$budget_mat_int_gepl,
			'budget_mat_ext_gepl'	=>	$budget_mat_ext_gepl
		);
		$rerr=validate_budgetrahmen(0, $budgetrahmen, $pdat, $iea, $pma);
		if($rerr)
			return $rerr;
	}

	// Genehmigtes Budget: Automatische Genehmigung der Aufgaben, dann ist das genehmigte Budget gleich dem geplanten Budget
	if(($pra == FXP_PRJ_TASK) && !$budgetgen)
	{
		foreach($pma as $pm)
		{
			foreach($iea as $ie)
			{
				$gepl=${'budget_'.$pm.'_'.$ie.'_gepl'};
				$gen=${'budget_'.$pm.'_'.$ie.'_gen'};
				if($gepl != $gen)
				{
					${'budget_'.$pm.'_'.$ie.'_gen'}=$gepl;
					$detail_change=true;
				}
			}
		}
	}

	// Aktiviertes Budget
	if($pra == FXP_PRJ_TASK)
	{
		// Aufgabe aktiviert oder abgeschlossen, dann ist das aktivierte Budget gleich dem genehmigten Budget
		if(($pst == FXP_PS_ACTIVE) || ($pst == FXP_PS_COMPLETED))
		{
			$budget_pers_int_akt=$budget_pers_int_gen;
			$budget_pers_ext_akt=$budget_pers_ext_gen;
			$budget_mat_int_akt=$budget_mat_int_gen;
			$budget_mat_ext_akt=$budget_mat_ext_gen;
		}
		else
		{
			$budget_pers_int_akt=0.0;
			$budget_pers_ext_akt=0.0;
			$budget_mat_int_akt=0.0;
			$budget_mat_ext_akt=0.0;
		}
	}

	// Genehmigtes Budget: Bei aktivierter Budgetgenehmigung (= keine Autogenehmigung) prüfen, ob Budgets genehmigt wurden bevor Aufgaben aktiviert werden können (Ausnahme: Terminaufgaben)
	if(($pra == FXP_PRJ_TASK) && $budgetgen && $budgetgenaktiv && ($pst == FXP_PS_ACTIVE) && ($pki != 1921) && (!sizeof($old) || (sizeof($old) && !$old['budget_pers_int_gen'] && !$old['budget_pers_ext_gen'] && !$old['budget_mat_int_gen'] && !$old['budget_mat_ext_gen'])))
		return 2354;	// Diese Aufgabe darf noch nicht <b>aktiviert</b> werden, da hierzu noch kein genehmigtes Budget vorliegt!

	// Verplantes Personenbudget:
	if($pra == FXP_PRJ_TASK)
	{
		// Verplantes Int. Personenbudget:
		// = Summe der (noch) nicht bezahlten Personeneingangsrechnungen (3099)
		$sql  = "SELECT SUM(nettobetrag) AS netto FROM rechnungseingang WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
		$sql .= " AND budgettyp=3099 AND (nettobetrag IS NOT NULL AND nettobetrag>0) AND bezahlt_am IS NULL";
		$budget_pers_int_verpl=get_double(db_value($sql));
//fxDebug($budget_pers_int_verpl, '$budget_pers_int_verpl (Unbezahlte Eingangsrechnungen): '.$sql);

		// Verplantes Ext. Personenbudget:
		$budget_pers_ext_verpl=0.0;
	}

	// Verplantes Materialbudget:
	if($pra == FXP_PRJ_TASK)
	{
		// Verplantes Int. Materialbudget:
		// = Summe der Plankosten aller "bestellten" (1238) Sofort-Materialien (3149)
		$sql  = "SELECT SUM(soll_kosten * sachmenge) AS netto FROM sachmittel WHERE fuervorgang IN (".$vinf.") AND mandanten_id=".$mid;
		$sql .= " AND sachmittelstatus=1238 AND verwendungsart=3149 AND (soll_kosten IS NOT NULL AND soll_kosten>0) AND (sachmenge IS NOT NULL AND sachmenge>0)";
		$bmvp=get_double(db_value($sql));
//fxDebug($bmvp, '$budget_mat_int_verpl (Sofort-Material): '.$sql);
		// + Summe der (noch) nicht bezahlten Materialeingangsrechnungen (3100)
		$sql  = "SELECT SUM(nettobetrag) AS netto FROM rechnungseingang WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
		$sql .= " AND budgettyp=3100 AND (nettobetrag IS NOT NULL AND nettobetrag>0) AND bezahlt_am IS NULL";
		$bimvp=get_double(db_value($sql));
//fxDebug($bimvp, '$budget_mat_int_verpl (Unbezahlte Eingangsrechnungen): '.$sql);
		$budget_mat_int_verpl=$bmvp+$bimvp;

		// Verplantes Ext. Materialbudget:
		$budget_mat_ext_verpl=0.0;
		// = Summe der Verkaufspreise aller "bestellten" (1238) Sofort-Materialien (3149)
		$sql  = "SELECT SUM(verkaufspreis * sachmenge) AS netto FROM sachmittel WHERE fuervorgang IN (".$vinf.") AND mandanten_id=".$mid;
		$sql .= " AND sachmittelstatus=1238 AND verwendungsart=3149 AND (verkaufspreis IS NOT NULL AND verkaufspreis>0) AND (sachmenge IS NOT NULL AND sachmenge>0)";
		$budget_mat_ext_verpl=get_double(db_value($sql));
//fxDebug($budget_mat_ext_verpl, '$budget_mat_ext_verpl (Sofort-Material): '.$sql);
	}

	// Abgerechnete Beträge ermitteln
	$sqlg  = "SELECT SUM(rp.gesamtpreis) AS netto FROM rechnungsposition rp, rechnung r";
	$sqlg .= " WHERE r.rechnungs_id=rp.rechnungs_id AND r.mandanten_id=".$mid." AND rp.mandanten_id=".$mid." AND rp.projekt_id IN (".$vinf.") AND (r.archiv=0 OR r.archiv IS NULL)";
	// ...50=FP (Personen), 51=SB (Personen), 52=RK, 53=MT (Sofort), 54=FP (Material), 55=MT (Zeit), 56= MT (Verbrauch)
	for($sma=50; $sma<57; $sma++)
	{
		$sql=$sqlg." AND (rp.ist_sachmittel=".$sma.")";
		${'r'.$sma}=get_double(db_value($sql));
//fxDebug(${'r'.$sma}, '$r'.$sma.': '.$sql);
	}

	// Verbrauchtes Personenbudget
	if($pra == FXP_PRJ_TASK)
	{
		// Verbrauchtes Int. Personalbudget:
		// ...Summe der EK der Zeiterfassung
		$sql  = "SELECT SUM(kosten_ek) AS netto FROM zeitdaten WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
		$sql .= " AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.") AND (kosten_ek IS NOT NULL AND kosten_ek>0)";
		$bpek=get_double(db_value($sql));
//fxDebug($bpek, '$budget_pers_int_verbr (Zeiterfassung): '.$sql);
		// ...Summe der EK der Reisekostenerfassung
		$sql  = "SELECT SUM(rk_ek) AS netto FROM zeitdaten WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
		$sql .= " AND zeitart IN (".FXP_TE_TRAVELEXP_TIMEREC.",".FXP_TE_TRAVELEXP.") AND (rk_ek IS NOT NULL AND rk_ek>0)";
		$brek=get_double(db_value($sql));
//fxDebug($brek, '$budget_pers_int_verbr (Reisekosten): '.$sql);
		// ...Summe der bezahlten Personaleingangsrechnungen
		$sql  = "SELECT SUM(nettobetrag) AS netto FROM rechnungseingang WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
		$sql .= " AND budgettyp=3099 AND (nettobetrag IS NOT NULL AND nettobetrag>0) AND bezahlt_am IS NOT NULL";
		$bipek=get_double(db_value($sql));
//fxDebug($bipek, '$budget_pers_int_verbr (Eingangsrechnungen): '.$sql);
		$budget_pers_int_verbr=$bpek+$brek+$bipek;

		// Verbrauchtes Ext. Personalbudget:
		// ...Summe der VK der abrechenbaren Zeiterfassung auf Festpreis- oder Stundenbasisaufgaben
		$bpvk=0.0;
		if(($abrechnungsart == FXP_INV_FIX) || ($abrechnungsart == FXP_INV_HOUR))
		{
			$sql  = "SELECT SUM(kosten_vk) AS netto FROM zeitdaten WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
			$sql .= " AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.") AND (kosten_vk IS NOT NULL AND kosten_vk>0) AND (nicht_abrechenbar IS NULL OR nicht_abrechenbar=0)";
			$bpvk=get_double(db_value($sql));
		}
//fxDebug($bpvk, '$budget_pers_ext_verbr (Zeiterfassung): '.$sql);
		// ...Summe der VK der abrechenbaren Reisekostenerfassung
		$sql  = "SELECT SUM(rk_vk) AS netto FROM zeitdaten WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
		$sql .= " AND zeitart IN (".FXP_TE_TRAVELEXP_TIMEREC.",".FXP_TE_TRAVELEXP.") AND (rk_vk IS NOT NULL AND rk_vk>0) AND (rk_nicht_abrechenbar IS NULL OR rk_nicht_abrechenbar=0)";
		$brvk=get_double(db_value($sql));
//fxDebug($brvk, '$budget_pers_ext_verbr (Reisekosten): '.$sql);
		$budget_pers_ext_verbr=$bpvk+$brvk;
	}

	// Verbrauchtes Materialbudget
	if($pra == FXP_PRJ_TASK)
	{
		// Verbrauchtes Int. Materialbudget:
		// ...Summe der Anschaffungskosten aller "bezahlten" Sofort-Materialien
		$sql  = "SELECT SUM(anschaffungspreis * sachmenge) AS netto FROM sachmittel WHERE fuervorgang=".$pid." AND mandanten_id=".$mid;
		$sql .= " AND sachmittelstatus=1237 AND verwendungsart=3149 AND (anschaffungspreis IS NOT NULL AND anschaffungspreis>0) AND (sachmenge IS NOT NULL AND sachmenge>0)";
		$bmek=get_double(db_value($sql));
//fxDebug($bmek, '$budget_mat_int_verbr (Sofort-Material): '.$sql);
		// ...Summe der EK des Materialverbrauchs
		$sql  = "SELECT SUM(kosten_ek * sachmenge) AS netto FROM materialverbrauch WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
		$sql .= " AND (kosten_ek IS NOT NULL AND kosten_ek>0) AND (sachmenge IS NOT NULL AND sachmenge>0)";
		$bvmek=get_double(db_value($sql));
//fxDebug($bvmek, '$budget_mat_int_verbr (Verbrauchs- und Zeit-Material): '.$sql);
		// ...Summe der bezahlten Materialeingangsrechnungen
		$sql  = "SELECT SUM(nettobetrag) AS netto FROM rechnungseingang WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
		$sql .= " AND budgettyp=3100 AND (nettobetrag IS NOT NULL AND nettobetrag>0) AND bezahlt_am IS NOT NULL";
		$bimek=get_double(db_value($sql));
//fxDebug($bimek, '$budget_mat_int_verbr (Eingangsrechnungen): '.$sql);
		$budget_mat_int_verbr=$bmek+$bvmek+$bimek;

		// Verbrauchtes Ext. Materialbudget:
		// ...Summe der Verkaufspreise aller "bezahlten" Sofort-Materialien
		$sql  = "SELECT SUM(verkaufspreis * sachmenge) AS netto FROM sachmittel WHERE fuervorgang=".$pid." AND mandanten_id=".$mid;
		$sql .= " AND sachmittelstatus=1237 AND verwendungsart=3149 AND (verkaufspreis IS NOT NULL AND verkaufspreis>0) AND (sachmenge IS NOT NULL AND sachmenge>0)";
		$bmvk=get_double(db_value($sql));
//fxDebug($bmvk, '$budget_mat_ext_verbr (Sofort-Material): '.$sql);
		// ...Summe der VK des abrechenbaren Materialverbrauchs auf Festpreis-, Stunden- oder Mengenbasisaufgaben
		$bvmvk=0.0;
		if(($abrechnungsart_mat == FXP_INV_FIX) || ($abrechnungsart_mat == FXP_INV_MAT_HU))
		{
			$sql  = "SELECT SUM(kosten_vk * sachmenge) AS netto FROM materialverbrauch WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
			$sql .= " AND (kosten_vk IS NOT NULL AND kosten_vk>0) AND (sachmenge IS NOT NULL AND sachmenge>0) AND (nicht_abrechenbar IS NULL OR nicht_abrechenbar=0)";
			$bvmvk=get_double(db_value($sql));
//fxDebug($bvmvk, '$budget_mat_ext_verbr (Verbrauchs- und Zeit-Material): '.$sql);
		}
		$budget_mat_ext_verbr=$bmvk+$bvmvk;
	}

	// Abgerechnetes Budget
	if($pra == FXP_PRJ_TASK)
	{
		// ...Abgerechnetes Ext. Personalbudget: Abgerechnete Personalkosten aus FP[Personen]=50, SB[Personen]=51 + RK=52
		$budget_pers_ext_abger=$r50+$r51+$r52;
		// ...Abgerechnetes Ext. Materialbudget: Abgerechnete Materialkosten aus MT[Sofort]=53, FP[Material]=54, MT[Zeit]=55 + MT[Verbrauch]=56
		$budget_mat_ext_abger=$r53+$r54+$r55+$r56;
	}

	// PRÜFUNG: Verplantes + verbrauchtes Budget darf nicht grösser sein als das genehmigte Budget, falls in Admineinstellung so festgelegt
	// (ausser bei Rechnungsstellung oder falls das neue verbrauchte Budget kleiner ist als das bisherige)
	if(($GLOBALS['fxpglobals']['tr'] != 104) && (strtoupper(substr($GLOBALS['fxpglobals']['settings']['vb_kleiner_gb'], 0, 1)) == 'J'))
	{
		if(sizeof($old))
		{
			$old_budget_pers_ver_int=$old['budget_pers_int_verpl']+$old['budget_pers_int_verbr'];
			$old_budget_mat_ver_int=$old['budget_mat_int_verpl']+$old['budget_mat_int_verbr'];
		}
		else
		{
			$old_budget_pers_ver_int=0.0;
			$old_budget_mat_ver_int=0.0;
		}
		$budget_pers_ver_int=$budget_pers_int_verpl+$budget_pers_int_verbr;
		$budget_mat_ver_int=$budget_mat_int_verpl+$budget_mat_int_verbr;
//echo('$budget_pers_ver_int='.$budget_pers_ver_int.', $budget_pers_int_gen='.$budget_pers_int_gen.', $old_budget_pers_ver_int='.$old_budget_pers_ver_int.'<br />$budget_mat_ver_int='.$budget_mat_ver_int.', $budget_mat_int_gen='.$budget_mat_int_gen.', $old_budget_mat_ver_int='.$old_budget_mat_ver_int.'<hr />');
		if((($budget_pers_ver_int > $old_budget_pers_ver_int) || ($budget_mat_ver_int > $old_budget_mat_ver_int)) && ((($budget_pers_ver_int > $budget_pers_int_gen) && ($budget_pers_int_gen > 0.0)) || (($budget_mat_ver_int > $budget_mat_int_gen) && ($budget_mat_int_gen > 0.0))))
			return 1164;	// Sie haben mehr internes Budget verplant und/oder verbraucht als genehmigt wurde! Bitte setzen Sie sich mit dem Projektleiter in Verbindung.

		if(sizeof($old))
		{
			$old_budget_pers_ver_ext=$old['budget_pers_ext_verpl']+$old['budget_pers_ext_verbr'];
			$old_budget_mat_ver_ext=$old['budget_mat_ext_verpl']+$old['budget_mat_ext_verbr'];
		}
		else
		{
			$old_budget_pers_ver_ext=0.0;
			$old_budget_mat_ver_ext=0.0;
		}
		$budget_pers_ver_ext=$budget_pers_ext_verpl+$budget_pers_ext_verbr;
		$budget_mat_ver_ext=$budget_mat_ext_verpl+$budget_mat_ext_verbr;
//echo('$budget_pers_ver_ext='.$budget_pers_ver_ext.', $budget_pers_ext_gen='.$budget_pers_ext_gen.', $old_budget_pers_ver_ext='.$old_budget_pers_ver_ext.'<br />$budget_mat_ver_ext='.$budget_mat_ver_ext.', $budget_mat_ext_gen='.$budget_mat_ext_gen.', $old_budget_mat_ver_ext='.$old_budget_mat_ver_ext.'<hr />');
		if((($budget_pers_ver_ext > $old_budget_pers_ver_ext) || ($budget_mat_ver_ext > $old_budget_mat_ver_ext)) && ((($budget_pers_ver_ext > $budget_pers_ext_gen) && ($budget_pers_ext_gen > 0.0)) || (($budget_mat_ver_ext > $budget_mat_ext_gen) && ($budget_mat_ext_gen > 0.0))))
			return 1226;	// Sie haben mehr externes Budget verplant und/oder verbraucht als genehmigt wurde! Bitte setzen Sie sich mit dem Projektleiter in Verbindung.
	}

	// Abgerechnete Beträge (bereits oben ermittelt) zuordnen
	$rechnung_fest_ext	= $r50+$r54;		// FP		= Festpreis [Personen=50 + Material=54]
	$rechnung_std_ext	= $r51+$r55+$r56;	// SB/MB	= Stunden- oder Mengenbasis [Personen=51 + Zeit-Material=55 + Verbrauchs-Material=56]
	$rechnung_reise_ext	= $r52;				// RK		= Reisekosten [52]
	$rechnung_mat_ext	= $r53;				// MT		= Materialkosten [Sofort-Material=53]

	// Offene Rechnungsbeträge ermitteln
	if($pra == FXP_PRJ_TASK)
	{
		// Gesamtmögliche Beträge der Abrechnungsarten (483=FP, 639=SB, 645=MB, 640=NA) ermitteln
		// ...Personen/Material: Festpreis + Stunden-/Mengenbasis
		$rechnung_fest_ext_off=0.0;
		$rechnung_std_ext_off=0.0;
		// ...Personen
		switch($abrechnungsart)
		{
			case FXP_INV_FIX:	// Festpreis
				$rechnung_fest_ext_off += $budget_pers_ext_akt;
			break;

			case FXP_INV_HOUR:	// Stundenbasis
				// ...Zeiterfassung
				$sql  = "SELECT SUM(kosten_vk) AS netto FROM zeitdaten WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
				$sql .= " AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.") AND (kosten_vk IS NOT NULL AND kosten_vk>0) AND (nicht_abrechenbar IS NULL OR nicht_abrechenbar=0)";
				$rechnung_std_ext_off += get_double(db_value($sql));
			break;
		}
		// ...Material
		switch($abrechnungsart_mat)
		{
			case FXP_INV_FIX:	// Festpreis
				$rechnung_fest_ext_off += $budget_mat_ext_akt;
			break;

			case FXP_INV_MAT_HU:	// Stunden- oder Mengenbasis
				// ...Zeit- oder Verbrauchsmaterial
				$sql  = "SELECT SUM(kosten_vk * sachmenge) AS netto FROM materialverbrauch WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
				$sql .= " AND (kosten_vk IS NOT NULL AND kosten_vk>0) AND (sachmenge IS NOT NULL AND sachmenge>0) AND (nicht_abrechenbar IS NULL OR nicht_abrechenbar=0)";
				$rechnung_std_ext_off += get_double(db_value($sql));
			break;
		}

		// ...Reisekosten
		$sql  = "SELECT SUM(rk_vk) AS netto FROM zeitdaten WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
		$sql .= " AND zeitart IN (".FXP_TE_TRAVELEXP_TIMEREC.",".FXP_TE_TRAVELEXP.") AND (rk_vk IS NOT NULL AND rk_vk>0) AND (rk_nicht_abrechenbar IS NULL OR rk_nicht_abrechenbar=0)";
		$rechnung_reise_ext_off=get_double(db_value($sql));
//fxDebug($rechnung_reise_ext_off, '$rechnung_reise_ext_off: '.$sql);

		// ...Material (Sofort-Material)
		$sql  = "SELECT SUM(verkaufspreis * sachmenge) AS netto FROM sachmittel WHERE fuervorgang=".$pid." AND mandanten_id=".$mid;
		$sql .= " AND sachmittelstatus=1237 AND verwendungsart=3149 AND soll_ende IS NOT NULL AND (verkaufspreis IS NOT NULL AND verkaufspreis>0) AND (sachmenge IS NOT NULL AND sachmenge>0)";
		$rechnung_mat_ext_off=get_double(db_value($sql));
//fxDebug($rechnung_mat_ext_off, '$rechnung_mat_ext_off: '.$sql);

		// Offene Rechnungsbeträge=Gesamtmögliche Beträge minus abgerechneter Beträge
		$rechnung_fest_ext_off	= max(0.0, $rechnung_fest_ext_off-$rechnung_fest_ext);
		$rechnung_std_ext_off	= max(0.0, $rechnung_std_ext_off-$rechnung_std_ext);
		$rechnung_reise_ext_off	= max(0.0, $rechnung_reise_ext_off-$rechnung_reise_ext);
		$rechnung_mat_ext_off	= max(0.0, $rechnung_mat_ext_off-$rechnung_mat_ext);
	}

	// Prüfen, ob aktiviertes externes Budget kleiner dem bereits abgerechneten Budget ist? (relevant für Festpreisaufgaben)
	// (ausser bei Rechnungsstellung)
	if(($GLOBALS['fxpglobals']['tr'] != 104) && ($pra == FXP_PRJ_TASK) && ($pst == FXP_PS_ACTIVE) && (($budget_pers_ext_akt < $r50) || ($budget_mat_ext_akt < $r54)))
		return 2749;	// Das extern aktivierte Budget darf nicht kleiner sein wie der bereits abgerechnete Betrag!

	// Evtl. Restbudgetfreigabe, falls Aufgabe abgeschlossen wird
	if(($pra == FXP_PRJ_TASK) && ($old_pst != $pst) && ($pst == FXP_PS_COMPLETED) && (strtoupper(substr($GLOBALS['fxpglobals']['settings']['budget_freigabe'], 0, 1)) == 'J'))
	{
		foreach($pma as $pm)
		{
			foreach($iea as $ie)
			{
				$bud_gepl	= 'budget_'.$pm.'_'.$ie.'_gepl';
				$bud_gen	= 'budget_'.$pm.'_'.$ie.'_gen';
				$bud_akt	= 'budget_'.$pm.'_'.$ie.'_akt';
				$bud_verbr	= 'budget_'.$pm.'_'.$ie.'_verbr';
				$bud_abger	= 'budget_'.$pm.'_'.$ie.'_abger';

				${$bud_gepl}=min(${$bud_gepl}, ${$bud_verbr});
				$old_bud_gen=${$bud_gen};
				if($ie == 'int')
					${$bud_gen}=max(${$bud_gen}, ${$bud_verbr});
				else
					${$bud_gen}=max(${$bud_gen}, ${$bud_abger});
				${$bud_akt}=${$bud_gen};

				if(!strlen($barr['textzeile']) && ($old_bud_gen != ${$bud_gen}))
				{
					$barr['textzeile']=meldung(2748, true);	// Restbudgetfreigabe bei Aufgabenabschluss
					$detail_change=true;
				}
			}
		}
	}

	// Deltawerte ermitteln=Aktueller Wert - Alter Wert
	$bfarr=array($bf_rahmen, $bf_plan, $bf_kalkuliert, $bf_rechnung);
	foreach($bfarr as $bn => $bfsubarr)
	{
		foreach($bfsubarr as $bsn => $fld)
		{
			${'delta_'.$fld}=${$fld};
			if(sizeof($old))
				${'delta_'.$fld} -= $old[$fld];
		}
	}

	// EK_voraus + VK_voraus (geplant bzw. genehmigt)
	$ek_voraus=0.0;
	$vk_voraus=0.0;
	if(($pra == FXP_PRJ_TASK) && ($aws != 0.0))
	{
		if($budget_pers_int_gen > 0.0)
			$ek_voraus=($budget_pers_int_gen * 3600.0)/$aws;
		else if($budget_pers_int_gepl > 0.0)
			$ek_voraus=($budget_pers_int_gepl * 3600.0)/$aws;

		if($budget_pers_ext_gen > 0.0)
			$vk_voraus=($budget_pers_ext_gen * 3600.0)/$aws;
		else if($budget_pers_ext_gepl > 0.0)
			$vk_voraus=($budget_pers_ext_gepl * 3600.0)/$aws;

		$ek_voraus=round($ek_voraus, $GLOBALS['fxpglobals']['settings']['nkstellen']);
		$vk_voraus=round($vk_voraus, $GLOBALS['fxpglobals']['settings']['nkstellen']);
	}

	// EK + VK
	$ek=0.0;
	$ek_change=false;
	if(isset($barr['ek']))
		$ek_change=true;
	$vk=0.0;
	$vk_change=false;
	if(isset($barr['vk']))
		$vk_change=true;
	if($pra == FXP_PRJ_TASK)
	{
		$ek=get_double($barr['ek']);
		if(!$ek)
			$ek=0.0;
		else
			$ek=round($ek, $GLOBALS['fxpglobals']['settings']['nkstellen']);
		$vk=get_double($barr['vk']);
		if(!$vk)
			$vk=0.0;
		else
			$vk=round($vk, $GLOBALS['fxpglobals']['settings']['nkstellen']);
	}

	// Textzeile
	$textzeile	= trim($barr['textzeile']);
	if(strlen($textzeile))
	{
		$textzeile="'".convert_string($textzeile, 'todb')."'";
		$detail_change=true;
	}
	else if($GLOBALS['autoupdate'])
		$textzeile="'Autom. Update ".$GLOBALS['version']."'";
	else
		$textzeile="'fx-project tr".$GLOBALS['fxpglobals']['tr']."'";
	$jetzt="'".$GLOBALS['datetime']."'";

	$berr	= false;
	$bfarr	= array($bf_plan, $bf_kalkuliert, $bf_rechnung);

	// budget_detail ...Neuanlage oder Ändern
	if($vex)
	{
		if($detail_change)
		{
			$budget_id=id_max('budget_detail', 'budget_id', "mandanten_id=".$mid, $mid);

			$chk  = "SELECT projekt_id FROM budget_detail WHERE budget_id=".$budget_id." AND mandanten_id=".$mid;
			$sql  = "INSERT INTO budget_detail";
			$sql .= " (budget_id, mandanten_id, projekt_id, budgettyp,";
			$sql .= " budgetrahmen_pers_int, budgetrahmen_pers_ext, budgetrahmen_mat_int, budgetrahmen_mat_ext,";
			$sql .= " budget_pers_int_gepl, budget_pers_ext_gepl, budget_mat_int_gepl, budget_mat_ext_gepl,";
			$sql .= " budget_pers_int_gen, budget_pers_ext_gen, budget_mat_int_gen, budget_mat_ext_gen,";
			$sql .= " gen_beginn, gen_ende, textzeile, anlage_dtm, angelegt_von) ";
			$sql .= "VALUES";
			$sql .= " (".$budget_id.", ".$mid.", ".$pid.", ".$budgettyp.",";
			$sql .= " ".$delta_budgetrahmen_pers_int.", ".$delta_budgetrahmen_pers_ext.", ".$delta_budgetrahmen_mat_int.",".$delta_budgetrahmen_mat_ext.",";
			$sql .= " ".$delta_budget_pers_int_gepl.", ".$delta_budget_pers_ext_gepl.", ".$delta_budget_mat_int_gepl.", ".$delta_budget_mat_ext_gepl.",";
			$sql .= " ".$delta_budget_pers_int_gen.", ".$delta_budget_pers_ext_gen.", ".$delta_budget_mat_int_gen.", ".$delta_budget_mat_ext_gen.",";
			$sql .= " ".$gen_beginn.", ".$gen_ende.", ".$textzeile.", ".$jetzt.", ".$GLOBALS['fxpglobals']['person'].")";
			if(($GLOBALS['__DGBLVL'] == 4) || ($_GET['test'] == '1969'))
				fxDebug($sql, $chk.' - Zeile: '.__LINE__);
			$berr=db_x($sql, 0, false, $change_aendzeit, $make_history);

			// Zeitdaten mitändern, falls vorhanden und verschieden vom alten Wert
			if(!$berr)
			{
				$chk="SELECT zeit_id, gen_beginn, gen_ende FROM zeitdaten WHERE projekt_id=".$pid." AND mandanten_id=".$mid." AND zeitart=102";
				$zid=db_values($chk);
				if(is_array($zid))
				{
					$old_gen_beginn='NULL';
					if(strlen($zid['gen_beginn']))
						$old_gen_beginn="'".$zid['gen_beginn']."'";
					$old_gen_ende='NULL';
					if(strlen($zid['gen_ende']))
						$old_gen_ende="'".$zid['gen_ende']."'";
					if(($old_gen_beginn != $gen_beginn) || ($old_gen_ende != $gen_ende))
					{
						$chk="SELECT zeit_id FROM zeitdaten WHERE projekt_id=".$pid." AND mandanten_id=".$mid." AND zeitart=102";
						$sql="UPDATE zeitdaten SET gen_beginn=".$gen_beginn.", gen_ende=".$gen_ende." WHERE projekt_id=".$pid." AND mandanten_id=".$mid." AND zeitart=102";
						if($GLOBALS['__DGBLVL'] == 4)
							fxDebug($sql, $chk.' - Zeile: '.__LINE__);
						$berr=db_x($sql, 0, false, $change_aendzeit, $make_history);
					}
				}
			}
		}
	}
	// budget_detail ...Löschen
	else
	{
		$chk="SELECT budget_id FROM budget_detail WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
		$sql="DELETE FROM budget_detail WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
		if(($GLOBALS['__DGBLVL'] == 4) || ($_GET['test'] == '1969'))
			fxDebug($sql, $chk.' - Zeile: '.__LINE__);
		$ckv=(int)db_value($chk);
		$berr=false;
		if($ckv)
			$berr=db_x($sql, 0, false, $change_aendzeit, $make_history);

		// Zeitdaten sind normalerweise schon gelöscht, ansonsten hier ändern falls verschieden vom alten Wert
		if(!$berr)
		{
			$chk="SELECT zeit_id, gen_beginn, gen_ende FROM zeitdaten WHERE projekt_id=".$pid." AND mandanten_id=".$mid." AND zeitart=102";
			$zid=db_values($chk);
			if(is_array($zid))
			{
				$old_gen_beginn='NULL';
				if(strlen($zid['gen_beginn']))
					$old_gen_beginn="'".$zid['gen_beginn']."'";
				$old_gen_ende='NULL';
				if(strlen($zid['gen_ende']))
					$old_gen_ende="'".$zid['gen_ende']."'";
				if(($old_gen_beginn != 'NULL') || ($old_gen_ende != 'NULL'))
				{
					$chk="SELECT zeit_id FROM zeitdaten WHERE projekt_id=".$pid." AND mandanten_id=".$mid." AND zeitart=102";
					$sql="UPDATE zeitdaten SET gen_beginn=NULL, gen_ende=NULL WHERE projekt_id=".$pid." AND mandanten_id=".$mid." AND zeitart=102";
					if(($GLOBALS['__DGBLVL'] == 4) || ($_GET['test'] == '1969'))
						fxDebug($sql, $chk.' - Zeile: '.__LINE__);
					$berr=db_x($sql, 0, false, $change_aendzeit, $make_history);
				}
			}
		}
	}

	// budget_summe ...Neuanlage
	if(!$berr && $vex && !sizeof($old))
	{
		$chk  = "SELECT mandanten_id FROM budget_summe WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
		$sql  = "INSERT INTO budget_summe";
		$sql .= " (projekt_id, mandanten_id, projektart, abrechnungsart, abrechnungsart_mat, budgettyp,";
		$sql .= " ek_voraus, vk_voraus, ek, vk,";
		$sql .= " budgetrahmen_pers_int, budgetrahmen_pers_ext, budgetrahmen_mat_int, budgetrahmen_mat_ext,";
		$sql .= " budget_pers_int_gepl, budget_pers_ext_gepl, budget_mat_int_gepl, budget_mat_ext_gepl,";
		$sql .= " budget_pers_int_gen, budget_pers_ext_gen, budget_mat_int_gen, budget_mat_ext_gen,";
		$sql .= " budget_pers_int_akt, budget_pers_ext_akt, budget_mat_int_akt, budget_mat_ext_akt,";
		$sql .= " budget_pers_int_verpl, budget_pers_ext_verpl, budget_mat_int_verpl, budget_mat_ext_verpl,";
		$sql .= " budget_pers_int_verbr, budget_pers_ext_verbr, budget_mat_int_verbr, budget_mat_ext_verbr,";
		$sql .= " budget_pers_ext_abger, budget_mat_ext_abger,";
		$sql .= " rechnung_fest_ext, rechnung_std_ext, rechnung_reise_ext, rechnung_mat_ext,";
		$sql .= " rechnung_fest_ext_off, rechnung_std_ext_off, rechnung_reise_ext_off, rechnung_mat_ext_off) ";
		$sql .= "VALUES";
		$sql .= " (".$pid.", ".$mid.", ".$pra.", ".$abrechnungsart.", ".$abrechnungsart_mat.", ".$budgettyp.",";
		$sql .= " ".$ek_voraus.", ".$vk_voraus.", ".$ek.", ".$vk.",";
		$sql .= " ".$budgetrahmen_pers_int.", ".$budgetrahmen_pers_ext.", ".$budgetrahmen_mat_int.", ".$budgetrahmen_mat_ext.",";
		$sql .= " ".$budget_pers_int_gepl.", ".$budget_pers_ext_gepl.", ".$budget_mat_int_gepl.", ".$budget_mat_ext_gepl.",";
		$sql .= " ".$budget_pers_int_gen.", ".$budget_pers_ext_gen.", ".$budget_mat_int_gen.", ".$budget_mat_ext_gen.",";
		$sql .= " ".$budget_pers_int_akt.", ".$budget_pers_ext_akt.", ".$budget_mat_int_akt.", ".$budget_mat_ext_akt.",";
		$sql .= " 0, 0, 0, 0,";
		$sql .= " 0, 0, 0, 0,";
		$sql .= " 0, 0,";
		$sql .= " 0, 0, 0, 0,";
		$sql .= " ".$rechnung_fest_ext_off.", ".$rechnung_std_ext_off.", ".$rechnung_reise_ext_off.", ".$rechnung_mat_ext_off.")";
		if(($GLOBALS['__DGBLVL'] == 4) || ($_GET['test'] == '1969'))
			fxDebug($sql, $chk.' - Zeile: '.__LINE__);
		$berr=db_x($sql, 0, false, $change_aendzeit, $make_history);
		if(!$berr && isset($GLOBALS['pid_buffer']) && is_array($GLOBALS['pid_buffer']))
			$GLOBALS['pid_buffer'][$pid]=true;
	}
	// budget_summe ...Ändern
	else if(!$berr && $vex && sizeof($old))
	{
		$chk="SELECT mandanten_id FROM budget_summe WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
		$fld='';
		// Projektart
		if($pra != $old['projektart'])
			$fld .= ", projektart=".$pra;
		// Abrechnungsart (Personen)
		if($abrechnungsart != $old['abrechnungsart'])
			$fld .= ", abrechnungsart=".$abrechnungsart;
		// Abrechnungsart (Material)
		if($abrechnungsart_mat != $old['abrechnungsart_mat'])
			$fld .= ", abrechnungsart_mat=".$abrechnungsart_mat;
		// Budgettyp
		if($budgettyp != $old['budgettyp'])
			$fld .= ", budgettyp=".$budgettyp;
		// EK_voraus
		if(round($ek_voraus, 4) != round($old['ek_voraus'], 4))
			$fld .= ", ek_voraus=".$ek_voraus;
		// VK_voraus
		if(round($vk_voraus, 4) != round($old['vk_voraus'], 4))
			$fld .= ", vk_voraus=".$vk_voraus;
		// EK
		if($ek_change && (round($ek, 4) != round($old['ek'], 4)))
			$fld .= ", ek=".$ek;
		// VK
		if($vk_change && (round($vk, 4) != round($old['vk'], 4)))
			$fld .= ", vk=".$vk;
		// Alle Budgetrahmenfelder
		foreach($bf_rahmen as $feldname)
		{
			if(round(${$feldname}, 4) != round($old[$feldname], 4))
				$fld .= ", ".$feldname."=".${$feldname};
		}
		// Alle restlichen Budgetfelder ausser Budgetrahmen
		foreach($bfarr as $bfsubarr)
		{
			foreach($bfsubarr as $feldname)
			{
				$v=${'delta_'.$feldname};
				if($v != 0.0)
				{
					$vz=' + ';
					if($v < 0.0)
					{
						$v =-$v;
						$vz=' - ';
					}
					$fld .= ", ".$feldname."=(".$feldname.$vz.$v.")";
				}
			}
		}
		if($fbud && !strlen($fld))
			$fld=", mandanten_id=".$mid;
		if(strlen($fld))
		{
			$sql="UPDATE budget_summe SET".substr($fld, 1)." WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
			if(($GLOBALS['__DGBLVL'] == 4) || ($_GET['test'] == '1969'))
				fxDebug($sql, $chk.' - Zeile: '.__LINE__);
			$berr=db_x($sql, 0, false, $change_aendzeit, $make_history);
			if(!$berr && isset($GLOBALS['pid_buffer']) && is_array($GLOBALS['pid_buffer']))
				$GLOBALS['pid_buffer'][$pid]=true;
		}
	}
	// budget_summe ...Löschen
	else if(!$berr && !$vex && sizeof($old))
	{
		$chk="SELECT mandanten_id FROM budget_summe WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
		$sql="DELETE FROM budget_summe WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
		if(($GLOBALS['__DGBLVL'] == 4) || ($_GET['test'] == '1969'))
			fxDebug($sql, $chk.' - Zeile: '.__LINE__);
		$ckv=(int)db_value($chk);
		$berr=false;
		if($ckv)
			$berr=db_x($sql, 0, false, $change_aendzeit, $make_history);
	}

	// budget_summe ...Ändern (Elter)
	if(!$berr && (sizeof($path) > 1))
	{
		foreach($path as $tpid => $p)
		{
			// Ausnahmen: Startvorgang, Keine vorhanden Budgetdaten (da z.B. bereits gelöscht)
			if(($tpid == $pid) || !sizeof($p['new']))
				continue;

			$fld='';
			foreach($p['new'] as $field => $value)
			{
				if((substr($field, 0, 7) == 'budget_') || (substr($field, 0, 9) == 'rechnung_'))
				{
					if(isset(${$field}) && ($p['typ']&2))
						$path[$tpid]['new'][$field] += ${$field};

					if($path[$tpid]['new'][$field] != $path[$tpid]['old'][$field])
						$fld .= ", ".$field."=".$path[$tpid]['new'][$field];
				}
			}

			if($fbud && !strlen($fld))
				$fld=", mandanten_id=".$mid;
			if(strlen($fld))
			{
				$chk="SELECT mandanten_id FROM budget_summe WHERE projekt_id=".$tpid." AND mandanten_id=".$mid;
				$sql="UPDATE budget_summe SET".substr($fld, 1)." WHERE projekt_id=".$tpid." AND mandanten_id=".$mid;
				if(($GLOBALS['__DGBLVL'] == 4) || ($_GET['test'] == '1969'))
					fxDebug($sql, $chk.' - Zeile: '.__LINE__);
				$berr=db_x($sql, 0, false, $change_aendzeit, $make_history);
				if($berr)
					break;
				else if(isset($GLOBALS['pid_buffer']) && is_array($GLOBALS['pid_buffer']))
					$GLOBALS['pid_buffer'][$tpid]=true;
			}
		}
	}

	// Ressourcenplanung anpassen für Schnittstelle
	if(!$berr && $fres)
	{
		$sql="SELECT maplanung_id FROM maplanung WHERE personen_id>0 AND projekt_id=".$pid." AND mandanten_id=".$mid;
		$res=db_value($sql);
		if($res)
		{
			$sql="UPDATE maplanung SET mandanten_id=".$mid." WHERE maplanung_id IN (".$sql.") AND mandanten_id=".$mid;
			$berr=db_x($sql, 0, false, $change_aendzeit, $make_history);
//fxDebug($berr, $sql);
		}
	}

	// 701: Ein Datenbankfehler ist aufgetreten in der Funktion zum Verwalten der Budgets!
	if($berr)
		$berr=701;

	return $berr;
}

/**
 * Funktion generiert bzw. liest für einen Vorgang einen eindeutigen PCA-Code
 *
 * @param various $pid       - Mandatory parameter:                   ???PARAMETER???
 * @param string  $def       - Optional parameter (default = ''):     ???PARAMETER???
 * @param boolean $ret_error - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function pca_code($pid, $def='', $ret_error=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pid=(int)$pid;
	if(!$pid)
		return 1805;	// Es wurde keine Projekt-ID übermittelt oder die Projekt-ID ist 0!

	$mid=$GLOBALS['fxpglobals']['client'];

	// Projektinfos
	$pex=false;
	$sql="SELECT vorgangsnummer FROM projekte WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
	$inf=db_values($sql);
//fxDebug($inf, $sql, 0);
	if(fxIsArray($inf))
	{
		$vn =no_null($inf['vorgangsnummer']);
		$pex=true;
	}

	// Fehlervariable
	$pca_err=false;

	// PCA-Code ermitteln
	$sql="SELECT parameter_txt FROM projekte_parameter WHERE projekt_id=".$pid." AND mandanten_id=".$mid." AND parameter_typ=376 AND parameter_var='TXT'";
	$pca=trim(db_value($sql));
//fxDebug($pca, $sql, 0);
	// ...vorhanden, aber kein Vorgang mehr, dann löschen
	if(strlen($pca) && !$pex)
	{
		$sql="DELETE FROM projekte_parameter WHERE projekt_id=".$pid." AND mandanten_id=".$mid." AND parameter_typ=376 AND parameter_var='TXT'";
		$pca_err=db_x($sql);
	}
	// ...nicht vorhanden, aber Vorgang, dann neu generieren
	else if(!strlen($pca) && $pex)
	{
		if(strlen($def))
			$ndbpca="'".convert_string($def, 'todb')."'";
		else
		{
			// Platzhalter
			$pca_platzhalter=trim($GLOBALS['fxpglobals']['settings']['pca']);
			$pca_ad=0;
			$pca_pa=array();
			$pca_tp='';
			for($i=0; $i<strlen($pca_platzhalter); $i++)
			{
				$c=substr($pca_platzhalter, $i, 1);
				// Korrekte Zeichen
				if(($c == 'P') || ($c == 'N') || ($c == 'Y') || ($c == 'M') || ($c == 'D') || ($c == 'h') || ($c == 'm') || ($c == 's') || ($c == 't') || ($c == 'n'))
				{
					if($c == $pca_tp)
						$pca_pa[$pca_ad]['len']++;
					else
					{
						if(is_array($pca_pa))
							$pca_ad++;

						$pca_pa[$pca_ad]=array('pos'=>$i, 'len'=>1, 'typ'=>$c);
						$pca_tp=$c;
					}
				}
				else
					$pca_tp='';
			}
//fxDebug($pca_pa, $pca_platzhalter, 0);

			$pcnt=0;
			while(true)
			{
				$ndbpca	= '';
				$pcnt++;
				// Maximal 100 Versuche erlauben um einen eindeutigen PCA-Code zu generieren
				if($pcnt > 100)
					break;

				if(!isset($GLOBALS['pca_lfdn']))
					$GLOBALS['pca_lfdn']=1;
				else
					$GLOBALS['pca_lfdn'] += 1;

				$npca=$pca_platzhalter;
				if(fxIsArray($pca_pa))
				{
					$t=time();
					list($usec, $sec)=explode(' ', microtime());

					foreach($pca_pa as $p)
					{
						$val	= substr($npca, $p['pos'], $p['len']);
						$c		= $p['typ'];
						switch($c)
						{
							case 'P':	// Interne Projekt-ID
								$val=substr(str_repeat('0', $p['len']).$pid, -$p['len']);
							break;

							case 'N':	// Vorgangsnummer
								$val=substr($vn.str_repeat(' ', $p['len']), 0, $p['len']);
							break;

							case 'Y':	// Jahr
								$val=substr(str_repeat('0', $p['len']).fx_date('Y', $t), -$p['len']);
							break;

							case 'M':	// Monat
								$val=substr(str_repeat('0', $p['len']).fx_date('m', $t), -$p['len']);
							break;

							case 'D':	// Tag
								$val=substr(str_repeat('0', $p['len']).fx_date('d', $t), -$p['len']);
							break;

							case 'h':	// Stunde
								$val=substr(str_repeat('0', $p['len']).fx_date('H', $t), -$p['len']);
							break;

							case 'm':	// Minute
								$val=substr(str_repeat('0', $p['len']).fx_date('i', $t), -$p['len']);
							break;

							case 's':	// Sekunde
								$val=substr(str_repeat('0', $p['len']).fx_date('s', $t), -$p['len']);
							break;

							case 't':	// Tausendstel-Sekunde
								$val=substr(substr($usec, 2).str_repeat('0', $p['len']), 0, $p['len']);
							break;

							case 'n':	// Fortlaufende Nummer
								$val=substr(str_repeat('0', $p['len']).$GLOBALS['pca_lfdn'], -$p['len']);
							break;
						}

						$npca=substr($npca, 0, $p['pos']).$val.substr($npca, $p['pos']+$p['len']);
					}
				}

				// Eindeutigkeit prüfen
				$npca=str_replace(' ', '', $npca);
				$ndbpca="'".convert_string($npca, 'todb')."'";
				$sql="SELECT parameter_id FROM projekte_parameter WHERE mandanten_id=".$mid." AND parameter_typ=376 AND parameter_var='TXT' AND parameter_txt=".$ndbpca;
				$chk=(int)db_value($sql);
//fxDebug($chk, $sql, 0);
				// ...nicht gefunden, dann anlegen
				if(!$chk)
					break;
			}
		}
//fxDebug($npca, $pcn, 0);

		// Neuen PCA-Code in DB schreiben
		if(strlen($ndbpca))
		{
	 		$paid=id_max('projekte_parameter', 'parameter_id');
			$sql  = "INSERT INTO projekte_parameter";
			$sql .= " (mandanten_id, parameter_id, projekt_id, parameter_typ, parameter_var, parameter_int, parameter_dec, parameter_txt, anlage_dtm, angelegt_von) ";
			$sql .= "VALUES";
			$sql .= " (".$mid.", ".$paid.", ".$pid.", 376, 'TXT', NULL, NULL, ".$ndbpca.", '".$GLOBALS['datetime']."', ".$GLOBALS['fxpglobals']['person'].")";

			$pca_err=db_x($sql, 0);
//fxDebug($sql, 'ERR='.$pca_err, 0);
			if(!$pca_err)
				$pca=$npca;
	 	}
	 	else
	 		$pca_err=true;
	}

	// PCA-Code oder Fehler zurückliefern
	if($ret_error)
	{
		if($pca_err)
			return 817;	// Fehler beim Verwalten des PCA-Codes!
		return false;
	}
	if($pca_err)
		return '?';
	return $pca;
}

/**
 * ???FUNCTION???
 *
 * @param various $vnl  - Mandatory parameter:                ???PARAMETER???
 * @param string  $feld - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function check_vn_range($vnl, $feld='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$vnl=(int)substr(trim($vnl), 0, 5);

	if(($vnl < 0) || ($vnl > 9999))
	{
		// Die Ganzzahl einer Vorgangsebene muss im Bereich zwischen 0 - 9999 liegen!
		if($feld)
		{
			if(is_array($feld))
				error_msg(1300, $feld);
			else if(strlen($feld))
				error_msg(1300, false, $feld);
			else
				error_msg(1300, false);
		}

		return true;
	}

	return false;
}

/**
 * ???FUNCTION???
 *
 * @param various $sdate - Mandatory parameter:                 ???PARAMETER???
 * @param various $edate - Mandatory parameter:                 ???PARAMETER???
 * @param integer $pst   - Optional parameter (default = 297):  ???PARAMETER???
 * @param integer $md    - Optional parameter (default = 365):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_datearray_range($sdate, $edate, $pst=297, $md=365)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sdate	= substr(trim($sdate), 0, 8);
	$edate	= substr(trim($edate), 0, 8);
	$pst	= (int)$pst;
//echo('get_datearray_range('.$sdate.', '.$edate.', '.$pst.', '.$md.');<br />');

	$heute	= substr($GLOBALS['datetime'],0,8);
	$hy		= (int)substr($heute,0,4);
	$hm		= (int)substr($heute,4,2);
	$mon	= (int)($md/30);
	if(!$mon)
		$mon=2;
	else if($mon%2 == 1)
		$mon++;
	$mon2	= (int)($mon/2);

	$hmoy	= fx_date('Ymd', fx_mktime(12, 0, 0, $hm-$mon, 1, $hy));
	$hmhy	= fx_date('Ymd', fx_mktime(12, 0, 0, $hm-$mon2, 1, $hy));
	$hphy	= fx_date('Ymd', fx_mktime(12, 0, 0, $hm+$mon2, 0, $hy));
	$hpoy	= fx_date('Ymd', fx_mktime(12, 0, 0, $hm+$mon, 0, $hy));
//echo('$hmoy: '.$hmoy.', $hmhy: '.$hmhy.' -- <B>$heute: '.$heute.'</B> -- $hphy: '.$hphy.', $hpoy: '.$hpoy.'<br />');

	// Vorgang noch nicht abgeschlossen und nicht inaktiv
	if(($pst != FXP_PS_COMPLETED) && ($pst != FXP_PS_INACTIVE))
	{
//		$sdate=min($sdate, $heute);
		$edate=max($edate, $heute);
	}

	// Vorgang dauert länger als ein Jahr
	$diff=time_diff($sdate, $edate, 'd');
//echo('$diff: '.$diff.'<br />');
	if($diff > $md)
	{
		if(($sdate < $hmhy) && ($edate > $hphy))
		{
			$sdate=$hmhy;
			$edate=$hphy;
		}
		else if($edate <= $heute)
		{
			$ey	= (int)substr($edate, 0, 4);
			$em	= (int)substr($edate, 4, 2);
			$nd	= fx_date('Ymd', fx_mktime(12, 0, 0, $em-$mon, 1, $ey));
			$sdate=max($sdate, $nd);
		}
		else if($sdate >= $heute)
		{
			$sy	= (int)substr($sdate, 0, 4);
			$sm	= (int)substr($sdate, 4, 2);
			$nd	= fx_date('Ymd', fx_mktime(12, 0, 0, $sm+$mon, 0, $sy));
			$edate=min($edate, $nd);
		}
		else if($sdate < $hmhy)
		{
			$ey	= (int)substr($edate, 0, 4);
			$em	= (int)substr($edate, 4, 2);
			$nd	= fx_date('Ymd', fx_mktime(12, 0, 0, $em-$mon, 1, $ey));
			$sdate=max($sdate, $nd);
		}
		else if($edate > $hphy)
		{
			$sy	= (int)substr($sdate, 0, 4);
			$sm	= (int)substr($sdate, 4, 2);
			$nd	= fx_date('Ymd', fx_mktime(12, 0, 0, $sm+$mon, 0, $sy));
			$edate=min($edate, $nd);
		}
	}

//echo('--> $sdate: '.$sdate.', $edate: '.$edate.'<br />');
	return array($sdate, $edate);
}

/**
 * gibt die Vorgangsebene zurück, zu der die übergebene Vorgangsnummer gehört
 * @param string $vorgangsnummer die Vorgangsnummer, deren Level ermittelt werden soll
 * @param int $relative wenn angegeben dann wird die Leveldifferenz zu diesem Level ermittelt
 * @return int die Vorgangsebene, entweder absolut oder relativ 
 */
function getLevel($vorgangsnummer, $relative=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($vorgangsnummer))
		$lvl=-1;
	else
		$lvl=substr_count($vorgangsnummer, '.') - $relative;

	return $lvl;
}

/**
 * gibt eine gültige Vorgangsnummer zurück
 * @param string $vorgaengerVN die Vorgangsnummer des Vorgangs von einer Zeile "höher"
 * @param int $level des Level des akt. Vorgangs, absolut zum Mutterprojekt gesehen
 * @param boolean $dbFormat gibt an in welchen Format die Vorgangsnummer geliefert und zurückgegeben werden soll 
 * @return string die gültige Vorgangsnummer, Format abhängig von $dbFormat
 */
function getValidVorgangsnummer($vorgaengerVN, $level, $dbFormat=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($dbFormat)
		$vorgaengerVN=no_null($vorgaengerVN);
	$vorgaengerLevel=getLevel($vorgaengerVN);
	// aktuelles Level größer (dann genau um eins, oder Verzeichnis ist fehlerhaft!)
	if($vorgaengerLevel < $level)
		$validVN=$vorgaengerVN.'.'.getFirstVNNumber();
	// aktuelles Level kleiner
	else if($vorgaengerLevel > $level)
	{
		$levels=explode('.', $vorgaengerVN);
		for($i=0; $i <= $level; $i++)
		{
			if($i == $level)
				$levels[$i] += 1;
			$newVN .= $levels[$i].".";
		}
		$validVN=substr($newVN, 0, -1);
	}
	// gleiche Levels
	else
	{
		$lastLevelPos=strrpos($vorgaengerVN, '.');
		$lastLevel=substr($vorgaengerVN, $lastLevelPos + 1);
		$validVN=substr($vorgaengerVN, 0, $lastLevelPos + 1).($lastLevel + 1);
	}
	if($dbFormat)
		$validVN=add_null($validVN);
	return $validVN;
}

/**
 * ???FUNCTION???
 *
 * @param various $pv  - Mandatory parameter:               ???PARAMETER???
 * @param integer $pid - Optional parameter (default = 0):  ???PARAMETER???
 * @param integer $def - Optional parameter (default = 0):  ???PARAMETER???
 * @param various $fb  - Mandatory parameter:               ???PARAMETER???
 * @param various $sb  - Mandatory parameter:               ???PARAMETER???
 *
 * @return ???RETURN???
 */
function pvr_select($pv, $pid=0, $def=0, $fb, $sb)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	global $nl;

	$fb=(int)$fb;
	$sb=(int)$sb;

	if(($GLOBALS['aktion'] == FXP_CREATE) || ($GLOBALS['aktion'] == FXP_CHANGE))
	{
		if(!strlen($GLOBALS['leer']))
			$GLOBALS['leer']=meldung(794, true);	// -leer-

		$id='pvr_'.$pv['mafaehigkeit_id'];

		$sql  = "SELECT p.personen_id, p.pname, p.vorname, p.inaktiv";
		$sql .= " FROM personen p, ma_art_faehigkeiten f";
		$sql .= " WHERE p.personen_id=f.personen_id AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND f.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND f.maart_id=2 AND f.mafaehigkeit_id=".$pv['mafaehigkeit_id'];
		$filter=filter_person('', false, false, $id, false, false);
		if(strlen($filter))
		{
			$apid=0;
			if(fxIsArray($GLOBALS['maskenfeldwerte']) && isset($GLOBALS['maskenfeldwerte'][$id]))
				$apid=(int)$GLOBALS['maskenfeldwerte'][$id];
			$sql .= ' AND ('.$filter;
			if($apid && ($GLOBALS['fxpglobals']['action'] != FXP_CREATE))
				$sql .= ' OR p.personen_id='.$apid;
			$sql .= ')';
		}
		$sql .= " ORDER BY p.pname, p.vorname";
		$par =db_values($sql, '*');
//fxDebug($par, $sql);

		$selarr=array(0=>$GLOBALS['leer']);
		if(fxIsArray($par))
		{
			// Bestand ermitteln
			if(!$def && $pid)
			{
				$sql  = "SELECT parameter_int FROM projekte_parameter";
				$sql .= " WHERE projekt_id=".(int)$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND parameter_typ=".$pv['mafaehigkeit_id']." AND parameter_var='INT' AND parameter_txt='PV'";
				$def =(int)db_value($sql);
//fxDebug($def, $sql);
			}

			$inakt=' {'.get_text(1391, '20').'}';	// {inaktiv}
			foreach($par as $pers)
			{
				$val=(int)$pers['personen_id'];

				$nam=trim($pers['pname']);
				$vnm=trim($pers['vorname']);
				if(strlen($vnm))
					$nam .= ', '.$vnm;
				if($pers['inaktiv'])
					$nam .= $inakt;

				$selarr[$val]=$nam;
			}
		}

		$GLOBALS[$id.'_ds_array']=$selarr;

		$zps ='<td nowrap width="'.$fb.'">'.$nl;
		$zps .= fieldSelect($id, $def, $sb);
		$zps .= '</td>'.$nl;
	}
	else
	{
		// Bestand ermitteln
		if(!$def && $pid)
		{
			$sql  = "SELECT parameter_int FROM projekte_parameter";
			$sql .= " WHERE projekt_id=".(int)$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND parameter_typ=".$pv['mafaehigkeit_id']." AND parameter_var='INT' AND parameter_txt='PV'";
			$def =(int)db_value($sql);
//fxDebug($def, $sql);
		}

		$zps='<td nowrap width="'.$fb.'">';
		if($def)
			$zps .= person_pic($def);
		else
			$zps .= '&nbsp;';
		$zps .= '</td>'.$nl;
	}

	return $zps;
}

/**
 * ???FUNCTION???
 *
 * @param various $pid    - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $pvarr  - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $delold - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function update_zusatzpv($pid, $pvarr=false, $delold=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pid=(int)$pid;
	if(!$pid)
		return 1805;	// Es wurde keine Projekt-ID übermittelt oder die Projekt-ID ist 0!

	$mid=$GLOBALS['fxpglobals']['client'];

	// Bestand ermitteln
	$old =array();
	$sql  = "SELECT parameter_typ, parameter_id, parameter_int FROM projekte_parameter";
	$sql .= " WHERE mandanten_id=".$mid." AND projekt_id=".$pid." AND parameter_var='INT' AND parameter_txt='PV'";
	$tmp =db_values($sql, '*');
//fxDebug($tmp, $sql);
	if(is_array($tmp))
	{
		foreach($tmp as $t)
			$old[(int)$t['parameter_typ']]=array('id'=>(int)$t['parameter_id'], 'int'=>(int)$t['parameter_int']);
	}
//fxDebug($old, '$old');

	// Übermittelte Werte durchgehen
	if(is_array($pvarr))
	{
		$jetzt=$GLOBALS['datetime'];

		foreach($pvarr as $typ => $val)
		{
			$typ=(int)$typ;
			$val=(int)$val;

			if($val)
			{
				// Neuanlage
				if(!is_array($old[$typ]))
				{
					$nid=id_max('projekte_parameter', 'parameter_id');

					$sql  = "INSERT INTO projekte_parameter";
					$sql .= " (mandanten_id, parameter_id, projekt_id, parameter_typ,";
					$sql .= " parameter_var, parameter_int, parameter_dec, parameter_txt,";
					$sql .= " anlage_dtm, angelegt_von) ";
					$sql .= "VALUES";
					$sql .= " (".$mid.", ".$nid.", ".$pid.", ".$typ.",";
					$sql .= " 'INT', ".$val.", NULL, 'PV',";
					$sql .= " '".$jetzt."', ".$GLOBALS['fxpglobals']['person'].")";
//fxDebug($sql, '$sql');

					$err=db_x($sql);
					if($err)
						return $err;
					else if(isset($GLOBALS['pid_buffer']) && is_array($GLOBALS['pid_buffer']))
						$GLOBALS['pid_buffer'][$pid]=true;
				}
				// Update
				else
				{
					if($val != $old[$typ]['int'])
					{
						$oid =$old[$typ]['id'];

						$sql  = "UPDATE projekte_parameter SET parameter_int=".$val." WHERE mandanten_id=".$mid." AND parameter_id=".$oid." AND projekt_id=".$pid;
//fxDebug($sql, '$sql');

						$err=db_x($sql);
						if($err)
							return $err;
						else if(isset($GLOBALS['pid_buffer']) && is_array($GLOBALS['pid_buffer']))
							$GLOBALS['pid_buffer'][$pid]=true;
					}

					unset($old[$typ]);
				}
			}
		}
	}

	// Veraltete Werte löschen
	if($delold && is_array($old))
	{
		foreach($old as $typ => $o)
		{
			$sql  = "DELETE FROM projekte_parameter WHERE mandanten_id=".$mid." AND parameter_id=".$o['id']." AND projekt_id=".$pid;
//fxDebug($sql, '$sql');

			$err=db_x($sql);
			if($err)
				return $err;
		}
	}

	return false;
}

/**
 * ???FUNCTION???
 *
 * @param various $pid - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function prj_get_resources($pid)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pid=(int)$pid;
	$mid=$GLOBALS['fxpglobals']['client'];
	$res=array();

	$sql  = "SELECT m.personen_id, p.pname, p.vorname, p.personentyp, p.geschlecht, m.auslastung, m.aufwand_std, m.prozent, a.ek_projekt, a.vk_projekt";
	$sql .= " FROM maplanung m, personen p, artikelzuordnung a";
	$sql .= " WHERE p.personen_id=m.personen_id AND p.mandanten_id=".$mid." AND a.maplanung_id=m.maplanung_id AND a.mandanten_id=".$mid;
	$sql .= " AND (m.personen_id IS NOT NULL AND m.personen_id>0) AND m.mandanten_id=".$mid." AND m.projekt_id=".$pid;
	$sql .= " ORDER BY p.pname, p.vorname";
	$tmp =db_values($sql, '*');
	if(is_array($tmp))
		$res=$tmp;

	return $res;
}

/**
 * ???FUNCTION???
 *
 * @param various $pid - Mandatory parameter:               ???PARAMETER???
 * @param integer $akt - Optional parameter (default = 3):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function prj_get_bg($pid, $akt=3)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mid=$GLOBALS['fxpglobals']['client'];

	$sql  = "SELECT personen_id FROM ma_art_faehigkeiten";
	$sql .= " WHERE maart_id=1 AND mafaehigkeit_id=-4 AND mandanten_id=".$mid;
	if($akt != FXP_CREATE)
		$sql .= " AND personen_id IN (".prights_pers($pid, false).")";
	$bgids=get_array($sql, 'personen_id');
//fxDebug($bgids, $sql);

	return $bgids;
}

/**
 * ???FUNCTION???
 *
 * @param various $oldpid - Mandatory parameter:                     ???PARAMETER???
 * @param various $newpid - Mandatory parameter:                     ???PARAMETER???
 * @param various $tid    - Mandatory parameter:                     ???PARAMETER???
 * @param string  $tname  - Optional parameter (default = ''):       ???PARAMETER???
 * @param string  $tstart - Optional parameter (default = ''):       ???PARAMETER???
 * @param string  $tend   - Optional parameter (default = ''):       ???PARAMETER???
 * @param array   $tres   - Optional parameter (default = array()):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function app_prj($oldpid, $newpid, $tid, $tname='', $tstart='', $tend='', $tres=array())
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$oldpid=(int)$oldpid;
	$newpid=(int)$newpid;

	// Terminaufgabe...
	// ...Neuanlage
	if(!$oldpid && $newpid)
	{
		$pid=app_prj_create($newpid, $tid, $tname, $tstart, $tend, $tres);
		if(!$pid)
			return false;

		return $pid;
	}
	// ...Löschen
	else if($oldpid)
	{
		$err=app_prj_delete($oldpid, $tid);
		if($err)
			return true;
	}

	return false;
}

/**
 * ???FUNCTION???
 *
 * @param various $pid - Mandatory parameter: ???PARAMETER???
 * @param various $tid - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function app_prj_delete($pid, $tid)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Übergabeparameter
	$pid_arr=array((int)$pid => $tid);
	$tid=(int)$tid;
	$mid=$GLOBALS['fxpglobals']['client'];

	// Bestandsdaten ermitteln
	$sql="SELECT mastertermin_id, personen_id, wiederholung FROM termine WHERE termin_id=".$tid." AND mandanten_id=".$mid;
	$act=db_values($sql); 
//fxDebug($act, $sql);
	if(!is_array($act))
		return true;

	// Terminaufgabe nur löschen, falls es sich um einen Mastertermin handelt oder es ein Wiederholungstermin ist
	$mid=(int)$act['mastertermin_id'];
	$wdh=(int)$act['wiederholung'];
	if(($tid == $mid) || ($wdh > 0))
	{
		// Falls es ein Mastertermin ist auch alle abhängigen Aufgaben ermitteln und löschen
		if($tid == $mid)
		{
			$sql="SELECT projekt_id, termin_id FROM termine WHERE mastertermin_id=".$mid." AND termin_id<>".$tid." AND mandanten_id=".$mid." AND zeit_id IS NOT NULL AND zeit_id<>0";
			$apr=db_values($sql, '*');
			if(is_array($apr))
			{
				foreach($apr as $a)
					$pid_arr[(int)$a['projekt_id']]=(int)$a['termin_id'];
			}
		}

		foreach($pid_arr as $pid => $ptid)
		{
			if(($GLOBALS['__DGBLVL'] > 2) && ($GLOBALS['__DGBLVL'] < 5))
				echo('<b>Terminaufgabe ['.$pid.'] löschen</b>...<br />');

			// Projektinfo holen
			$sql="SELECT vorgangsnummer, name_projekt, elter FROM projekte WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
			$dpinf=db_values($sql);

			// Prüfen, ob diese Aufgabe gelöscht werden darf
			$deleteable=checkDelProject($pid);
			if(!$deleteable)
				return true;

			// Aufgabe löschen
			$err=prj_db($pid);
			if($err)
				return true;

			// War dies die einzige Aufgabe unter dem Terminteilprojekt?
			$eid=(int)$dpinf['elter'];
			$sql="SELECT projekt_id FROM projekte WHERE elter=".$eid." AND mandanten_id=".$mid;
			$spe=(int)db_value($sql);
//fxDebug($spe, $sql);
			// ...ja, dann auch übergeordnetes Projekt löschen
			if(!$spe)
			{
				if(($GLOBALS['__DGBLVL'] > 2) && ($GLOBALS['__DGBLVL'] < 5))
					echo('<b>Terminprojekt ['.$eid.'] löschen</b>...<br />');

				$sql="SELECT elter, projekt_status FROM projekte WHERE projekt_id=".$eid." AND mandanten_id=".$mid;
				$eea=db_values($sql);
//fxDebug($eea, $sql);
				$err=prj_db($eid);
				if($err)
					return true;

				// ..evtl. muss der Projektstatus des übergeordneten Projektes angepasst werden, falls alle untergeordneten nun den gleichen Status haben
				if(is_array($eea))
				{
					$eeid=(int)$eea['elter'];
					$sql  = "SELECT DISTINCT projekt_status FROM projekte WHERE elter=".$eeid." AND mandanten_id=".$mid;
					$psa =db_values($sql, '*');
//fxDebug($psa, $sql);
					if(is_array($psa) && (sizeof($psa) == 1) && $psa[0]['projekt_status'] && ($psa[0]['projekt_status'] != $eea['projekt_status']))
					{
						$sql  = "UPDATE projekte SET projekt_status=".$psa[0]['projekt_status']." WHERE projekt_id=".$eeid." AND mandanten_id=".$mid;
						$err=db_x($sql);
//fxDebug($err, $sql);
						if(!$err)
							$err=set_status($eeid, $psa[0]['projekt_status']);

						if($err)
							return true;
					}
				}
			}
		}
	}
}

/**
 * ???FUNCTION???
 *
 * @param various $pid    - Mandatory parameter:                     ???PARAMETER???
 * @param various $tid    - Mandatory parameter:                     ???PARAMETER???
 * @param various $tname  - Mandatory parameter:                     ???PARAMETER???
 * @param various $tstart - Mandatory parameter:                     ???PARAMETER???
 * @param various $tend   - Mandatory parameter:                     ???PARAMETER???
 * @param array   $tres   - Optional parameter (default = array()):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function app_prj_create($pid, $tid, $tname, $tstart, $tend, $tres=array())
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Übergabeparameter
	$pid=(int)$pid;
	$tid=(int)$tid;
	$mid=$GLOBALS['fxpglobals']['client'];

	// Bestandsdaten ermitteln
	$sel="SELECT projekt_id, mutterprojekt_id, elter, vorgangsnummer, projektart, kategorie_id FROM projekte WHERE projekt_id=";
	$sql=$sel.$pid." AND mandanten_id=".$mid;
	$act=db_values($sql);
	if(!is_array($act))
		return false;

	// Falls Vorgang eine Aufgabe ist...
	if($act['projektart'] == FXP_PRJ_TASK)
	{
		// ...und eine Terminaufgabe, dann diese ID sofort zurückgeben...
		if($act['kategorie_id'] == 1921)
			return $act['projekt_id'];

		// ...ansonsten das Vorgängerprojekt ermitteln
		$pid=$act['elter'];
		$sql=$sel.$pid." AND mandanten_id=".$mid;
		$act=db_values($sql);
	}

	// Falls Projekt kein Terminprojekt ist...
	if($act['kategorie_id'] != 1921)
	{
		// ...dann prüfen, ob es bereits ein untergeordnetes Terminprojekt gibt...
		$sql="SELECT projekt_id FROM projekte WHERE elter=".$pid." AND mandanten_id=".$mid." AND kategorie_id=1921";
		$sid=(int)db_value($sql);
		// ...falls NEIN, dann Terminteilprojekt unter aktuelles Projekt mit Vorgangsnummer ...9999 erstellen
		if(!$sid)
		{
			$pname=$GLOBALS['fxpglobals']['settings']['name_terminpr'];
			$prpre=get_prefix_project($act['mutterprojekt_id']);
			if(strlen($prpre))
				$pname=$prpre.' '.$pname;

			$pparam=array
			(
				'mutterprojekt_id'	=> $act['mutterprojekt_id'],
				'elter'				=> $pid,
				'abhaengigkeit'		=> 0,
				'vorgangsnummer'	=> $act['vorgangsnummer'].".9999",
				'name_projekt'		=> $pname,
				'projektart'		=> FXP_PRJ_SUB,
				'projekt_status'	=> FXP_PS_ACTIVE,
				'beschreibung'		=> "",
				'anmerkung'			=> "",
				'anmerkungintern'	=> "",
				'ref_nummer'		=> "",
				'kategorie_id'		=> 1921,	// Termine
				'maart_id'			=> 0,
				'mafaehigkeit_id'	=> 0,
				'max_zeit_aufw'		=> 0.0,
				'aufwand_soll'		=> 0.0,
				'aufwand_ist'		=> 0.0,
				'aufwand_todo'		=> 0.0,
				'deadline'			=> ""
			);
			$zparam=array
			(
				'mutterprojekt_id'	=> $act['mutterprojekt_id'],
				'gen_beginn'		=> "",
				'gen_ende'			=> ""
			);
			if(($GLOBALS['__DGBLVL'] > 2) && ($GLOBALS['__DGBLVL'] < 5))
				echo('<b>Terminprojekt ['.$pparam['vorgangsnummer'].'] erstellen</b>...<br />');
			$sid=prj_db(0, $pparam, $zparam);
		}
	}
	else
		$sid=$pid;

	// Falls es ein Terminteilprojekt gibt...
	if($sid)
	{
		$sql=$sel.$sid;
		$act=db_values($sql);

		$nvn='';
		if(is_array($GLOBALS['tvn_buffer']) && sizeof($GLOBALS['tvn_buffer']))
		{
			if(isset($GLOBALS['tvn_buffer']['use']))
				unset($GLOBALS['tvn_buffer']['use']);

			reset($GLOBALS['tvn_buffer']);
			$avnlen=strlen($act['vorgangsnummer'])+5;
			foreach($GLOBALS['tvn_buffer'] as $avn => $apzid)
			{
				if(strlen($avn) == $avnlen)
				{
					$nvn=substr($avn, -4);
					$GLOBALS['tvn_buffer']['use']=$apzid;
					unset($GLOBALS['tvn_buffer'][$avn]);
					break;
				}
			}
		}

		if(!strlen($nvn))
		{
			$sql="SELECT vorgangsnummer FROM projekte WHERE elter=".$sid." AND mandanten_id=".$mid." AND vorgangsnummer NOT LIKE '*.9999' ORDER BY vorgangsnummer DESC";
			$mvn=db_value($sql);
//fxDebug($mvn, $sql);

			if(strlen($mvn))
			{
				$nvn=substr('0000'.(1+(int)substr($mvn, -4)), -4);
				if((int)$nvn > 9999)
					$nvn='9999';
			}
			else if(strtolower(substr($GLOBALS['fxpglobals']['settings']['vorgang0'], 0, 1)) == 'n')
				$nvn='0001';
			else
				$nvn='0000';
		}

		// ...dann eine neue Terminaufgabe darunter anlegen
		$prpre=get_prefix_project($act['mutterprojekt_id']);
		if(strlen($prpre))
			$tname=$prpre.' '.$tname;

		$pparam=array
		(
			'mutterprojekt_id'	=> $act['mutterprojekt_id'],
			'elter'				=> $sid,
			'abhaengigkeit'		=> 0,
			'vorgangsnummer'	=> $act['vorgangsnummer'].".".$nvn,
			'name_projekt'		=> $tname,
			'projektart'		=> FXP_PRJ_TASK,
			'projekt_status'	=> FXP_PS_ACTIVE,
			'beschreibung'		=> "",
			'anmerkung'			=> "",
			'anmerkungintern'	=> "",
			'ref_nummer'		=> "",
			'kategorie_id'		=> 1921,	// Termine
			'maart_id'			=> 0,
			'mafaehigkeit_id'	=> 0,
			'max_zeit_aufw'		=> 0.0,
			'aufwand_soll'		=> 0.0,
			'aufwand_ist'		=> 0.0,
			'aufwand_todo'		=> 0.0,
			'deadline'			=> ""
		);
		$zparam=array
		(
			'mutterprojekt_id'	=> $act['mutterprojekt_id'],
			'termin_id'			=> $tid,
			'soll_beginn'		=> $tstart,
			'soll_ende'			=> $tend,
			'gen_beginn'		=> "",
			'gen_ende'			=> ""
		);
		if(($GLOBALS['__DGBLVL'] > 2) && ($GLOBALS['__DGBLVL'] < 5))
			echo('<b>Terminaufgabe ['.$pparam['vorgangsnummer'].'] erstellen</b>...<br />');
		$aid=prj_db(0, $pparam, $zparam, $tres);

		// ...Projektstatus anpassen
		if($aid)
		{
			$err=set_status($aid, FXP_PS_ACTIVE);
			if($err)
				$aid=false;
		}
	}
	else
		$aid=false;

	return $aid;
}

/**
 * ???FUNCTION???
 *
 * @param various $timeid    - Mandatory parameter: ???PARAMETER???
 * @param various $projectid - Mandatory parameter: ???PARAMETER???
 * @param various $personid  - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function app_prj_timereg($timeid, $projectid, $personid)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$timeid		= (int)$timeid;
	$projectid	= (int)$projectid;
	$personid	= (int)$personid;
	$mid		= $GLOBALS['fxpglobals']['client'];
	$err		= false;

	$sql  = "SELECT t.termin_id, t.zeit_id FROM termine t, projekte p";
	$sql .= " WHERE t.projekt_id=".$projectid." AND t.personen_id=".$personid." AND t.mandanten_id=".$mid." AND t.zeit_id IS NOT NULL AND t.zeit_id<>0 AND t.bestaetigung_anmerk LIKE '1-%'";
	$sql .= " AND p.projekt_id=t.projekt_id AND p.mandanten_id=".$mid." AND p.kategorie_id=1921";
	$cva =db_values($sql);

	if(is_array($cva) && ($cva['zeit_id'] != $timeid))
	{
		// falls eine Zuordnung gelöscht werden soll prüfen, ob eine vorherige Zuordnung wiederhergestellt werden soll
		if($timeid < 0)
		{
			$sql="SELECT zeit_id FROM zeitdaten WHERE projekt_id=".$projectid." AND personen_id=".$personid." AND mandanten_id=".$mid." AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.",".FXP_TE_TRAVELEXP.") ORDER BY zeitstempel DESC";
			$ide=(int)db_value($sql);
			if($ide)
				$timeid=$ide;
		}
		$sql="UPDATE termine SET zeit_id=".$timeid." WHERE termin_id=".(int)$cva['termin_id']." AND mandanten_id=".$mid;
		$err=db_x($sql);
	}

	return $err;
}

/**
 * ???FUNCTION???
 *
 * @param various $pid    - Mandatory parameter:                     ???PARAMETER???
 * @param string  $pparam - Optional parameter (default = ''):       ???PARAMETER???
 * @param string  $zparam - Optional parameter (default = ''):       ???PARAMETER???
 * @param array   $tres   - Optional parameter (default = array()):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function prj_db($pid, $pparam='', $zparam='', $tres=array())
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// 0=Anzeigen,Anlegen,Ändern,Löschen
	$aaal=explode(',', meldung(16));

	// Standard-Projektparameter
	$pid	= (int)$pid;
	$mid	= $GLOBALS['fxpglobals']['client'];
	$err	= false;
	$now	= $GLOBALS['datetime'];
	$cause	= '';

	// ...Projekte
	$parr=array
	(
		'i|mandanten_id'		=> $mid,
		'i|projekt_id'			=> $pid,
		'i|mutterprojekt_id'	=> 0,
		'i|elter'				=> 0,
		'i|abhaengigkeit'		=> 0,
		'i|zeit_id'				=> 0,
		'i|budget_id'			=> 0,
		't|vorgangsnummer'		=> "0001",
		't|name_projekt'		=> '?',
		'i|projektart'			=> FXP_PRJ_MAIN,
		'i|projekt_status'		=> FXP_PS_PLANNED,
		'i|prioritaet'			=> 446,	// Mittel
		'i|projektmanager'		=> 0,
		'i|projektleiter'		=> 0,
		'i|vertreter'			=> 0,
		'i|kunde'				=> 0,
		'i|ansprechpartner'		=> 0,
		't|beschreibung'		=> "",
		't|anmerkung'			=> "",
		't|anmerkungintern'		=> "",
		't|ref_nummer'			=> "",
		'i|kategorie_id'		=> 0,
		'i|maart_id'			=> 0,
		'i|mafaehigkeit_id'		=> 0,
		'd|max_zeit_aufw'		=> 0.0,
		'd|aufwand_soll'		=> 0.0,
		'd|aufwand_ist'			=> 0.0,
		'd|aufwand_todo'		=> 0.0,
		'z|deadline'			=> "",
		'z|erstelldatum'		=> $now
	);

	// ...Zeitdaten
	$zarr=array
	(
		'i|mandanten_id'		=> $mid,
		'i|zuordnungs_mid'		=> $mid,
		'i|zeit_id'				=> 0,
		'i|projekt_id'			=> $pid,
		'i|mutterprojekt_id'	=> 0,
		'i|zeitart'				=> 102,
		'z|soll_beginn'			=> "",
		'z|soll_ende'			=> "",
		'z|gen_beginn'			=> "",
		'z|gen_ende'			=> ""
	);

	// Neuanlage
	if(!$pid)
	{
		// Projekteparameter ersetzen
		if(is_array($pparam) && $pparam['elter'])
		{
			$eid=(int)$pparam['elter'];
			$sql="SELECT * FROM projekte WHERE projekt_id=".$eid." AND mandanten_id=".$mid;
			$eparr=db_values($sql);
		}
		else
		{
			$eid=0;
			$eparr='';
		}
		foreach($parr as $pp => $pv)
		{
			$pp_typ=substr($pp, 0, 1);
			$pp_col=substr($pp, 2);

			if(is_array($pparam) && sizeof($pparam) && isset($pparam[$pp_col]))
				$pv=$pparam[$pp_col];
			else if(is_array($eparr) && sizeof($eparr) && isset($eparr[$pp_col]))
				$pv=$eparr[$pp_col];

			if($pp_typ == 'i')
				$parr[$pp]=(int)$pv;
			else if($pp_typ == 'd')
				$parr[$pp]=get_double($pv);
			else if(!strlen($pv))
				$parr[$pp]="NULL";
			else
				$parr[$pp]="'".convert_string($pv, 'todb')."'";
		}

		// Zeitparameter ersetzen
		if($eid)
		{
			$sql="SELECT * FROM zeitdaten WHERE projekt_id=".$eid." AND mandanten_id=".$mid." AND zeitart=102";
			$ezarr=db_values($sql);
		}
		else
			$ezarr='';
		foreach($zarr as $zp => $zv)
		{
			$zp_typ=substr($zp, 0, 1);
			$zp_col=substr($zp, 2);

			if(is_array($zparam) && sizeof($zparam) && isset($zparam[$zp_col]))
				$zv=$zparam[$zp_col];
			else if(is_array($ezarr) && sizeof($ezarr) && isset($ezarr[$zp_col]))
				$zv=$ezarr[$zp_col];

			if($zp_typ == 'i')
				$zarr[$zp]=(int)$zv;
			else if($zp_typ == 'd')
				$zarr[$zp]=get_double($zv);
			else if(!strlen($zv))
				$zarr[$zp]="NULL";
			else
				$zarr[$zp]="'".convert_string($zv, 'todb')."'";
		}

		// Validierung
		if(!$err)
		{
			// ...Passt neuer Vorgang in Vorgängerzeitraum
			if(is_array($ezarr) && sizeof($ezarr))
			{
				$estart=substr($ezarr['soll_beginn'], 0, 8);
				$eend	= substr($ezarr['soll_ende'], 0, 8);
				$tstart=substr($zarr['z|soll_beginn'], 1, 8);
//echo('$estart='.$estart.', $eend='.$eend.', $tstart='.$tstart.'<br />');
				if(($tstart < $estart) || ($tstart > $eend))
				{
					// 1928: Das Datum [$var1] dieser Terminaufgabe liegt ausserhalb des Zeitraums des zugeordneten Terminprojektes [$var2]. Bitte legen Sie einen anderen Terminzeitraum fest oder ändern Sie den Zeitraum des zugeordneten Terminprojektes.
					$var1=get_entry($tstart.'000000', 'datum');
					$var2=get_entry($estart.'000000', 'datum');
					if($eend != $estart)
						$var2 .= '-'.get_entry($eend.'240000', 'datum');
					error_msg(1928, false, '<b>'.$var1.'</b>', '<b>'.$var2.'</b>');
					$err=true;
				}
			}
		}

		// Neue ID´s
		if(!$err)
		{
			$pzid='';
			if(is_array($GLOBALS['tvn_buffer']) && sizeof($GLOBALS['tvn_buffer']))
			{
				$pvn=substr($parr['t|vorgangsnummer'], 1, -1);
				if(strlen($GLOBALS['tvn_buffer']['use']))
					$pzid=$GLOBALS['tvn_buffer']['use'];
				else if(strlen($GLOBALS['tvn_buffer'][$pvn]))
					$pzid=$GLOBALS['tvn_buffer'][$pvn];
			}

			if(strlen($pzid))
			{
				$pzid_arr=explode('|', $pzid);
				$pid=(int)$pzid_arr[0];
				$zid=(int)$pzid_arr[1];
			}
			else
			{
			  	$pid=id_max('projekte', 'projekt_id');
		  		$zid=id_max('zeitdaten', 'zeit_id');
		  	}
			$parr['i|projekt_id']=$pid;
			$zarr['i|projekt_id']=$pid;
			$parr['i|zeit_id']=$zid;
			$zarr['i|zeit_id']=$zid;
//fxDebug($parr, '$parr - New id='.$pid);
//fxDebug($zarr, '$zarr - New id='.$zid);

			// SQL's erzeugen
			$cols='';
			$vals='';
			foreach($parr as $pp => $pv)
			{
				if(strlen($cols))
				{
					$cols .= ", ";
					$vals .= ", ";
				}
				$cols .= substr($pp, 2);
				$vals .= $pv;
			}
			$sql="INSERT INTO projekte (".$cols.") VALUES (".$vals.")";
			$err=db_x($sql);
//fxDebug($sql, '$sql');
			if($err)
				$cause='projekte';
		}

		// Vorgangsnummer auf Eindeutigkeit prüfen
		if(!$err)
		{
			$err=check_vn($pid, false);
			if($err)
				$cause='projekte: nr';
		}

		// Zeitdaten
		if(!$err)
		{
			$cols='';
			$vals='';
			foreach($zarr as $zp => $zv)
			{
				if(strlen($cols))
				{
					$cols .= ", ";
					$vals .= ", ";
				}
				$cols .= substr($zp, 2);
				$vals .= $zv;
			}
			$sql="INSERT INTO zeitdaten (".$cols.") VALUES (".$vals.")";
			$err=db_x($sql);
//fxDebug($sql, '$sql');
			if($err)
				$cause='zeitdaten';
		}

		// Budget
		if(!$err)
		{
			$abrechnungsart		= FXP_INV_HOUR;		// Abrechnungsart (Personen)=Stundenbasis
			$abrechnungsart_mat	= FXP_INV_MAT_HU;	// Abrechnungsart (Material)=Mengenbasis

			$barr=array
			(
				'budgettyp' => 702, 'abrechnungsart' => $abrechnungsart, 'abrechnungsart_mat' => $abrechnungsart_mat,
				'budgetrahmen_pers_int'	=> 0.0,	'budgetrahmen_pers_ext'	=> 0.0,	'budgetrahmen_mat_int'	=> 0.0,	'budgetrahmen_mat_ext'	=> 0.0,
				'budget_pers_int_gepl'	=> 0.0,	'budget_mat_int_gepl'	=> 0.0,	'budget_pers_ext_gepl'	=> 0.0,	'budget_mat_ext_gepl'	=> 0.0
			);
			$err=update_budget($pid, $barr);
			if($err)
				error_msg($err, false);
		}

		// PCA-Code
		if(!$err)
		{
			$npca=pca_code($pid);
			if($npca == '?')
			{
				$err=true; 
				$cause='budget_detail: pca';
			}
		}

		// Fortschrittsmethode
		if(!$err)
		{
			$err=projektfortschritt($pid, 0, 0.0, 1784, 1790, 0, $parr['i|projektart']);
			if($err)
				$cause='projektfortschritt';
		}

		// Ressourcenplanung
		if(!$err)
		{
			$pstbck	= $_POST;
			$_POST	= array();
			if(is_array($tres) && sizeof($tres))
			{
				$ekvk0=get_entry(0, 'dezimal');
				foreach($tres as $psid => $trd)
				{
					$psid=(int)$psid;

					if(!isset($_POST['AZO'][$psid]))
					{
						$_POST['AZO'][$psid]	= 0;
						$_POST['OEK'][$psid]	= $ekvk0;
						$_POST['OVK'][$psid]	= $ekvk0;
						$_POST['OAUFW'][$psid]	= '';
					}
					// Standard-EK/VK
					$sql="SELECT ek, vk FROM artikel WHERE personen_id=".$psid." AND mandanten_id=".$mid." AND (sachmittel_id=0 OR sachmittel_id IS NULL) AND (archiv=0 OR archiv IS NULL)";
					$tmp=db_values($sql);
					$_POST['EK'][$psid]		= get_entry($tmp['ek'], 'dezimal');
					$_POST['VK'][$psid]		= get_entry($tmp['vk'], 'dezimal');
					$_POST['AUFW'][$psid]	= '';
					$_POST['OK'][$psid]		= 1;
				}
			}

			if(sizeof($_POST))
			{
				$rpid			= array(0=>$pid);
				$iau			= $pid;
				$iau_arr		= $rpid;
				$nosuccess_msg	= true;
				$_POST['MAZ']	= true;
//fxDebug('_POST', __FILE__.' ['.__LINE__.'] -- $_POST');

				require('ma_verfuegbarkeit_ini.inc');

				if($err)
					$cause='maplanung';
			}
			$_POST=$pstbck;
		}

		if($err)
		{
			if(strlen($cause))
			{
				// 1927: Fehler beim $var1 des Spezialprojektes "$var2"$var3
				error_msg(1927, false, '<b>'.$aaal[1].'</b> ['.$cause.']', '<b><font color=#0000AA>'.no_null(substr($parr['t|vorgangsnummer'], 1 ,-1)).'</font></b>&nbsp;&nbsp;<b>'.substr($parr['t|name_projekt'].'</b>', 1, -1), '!');
			}
			return false;
		}

		return $pid;
	}
	// Löschen
	else
	{
		$sql="SELECT zeit_id, vorgangsnummer, projektart, elter, aufwand_soll FROM projekte WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
		$his=db_values($sql);
//fxDebug($his, $sql);

		// Zusatzprojektverantwortliche löschen
		$err=update_zusatzpv($pid, false, true);
		if($err)
			$cause='projekt_parameter';

		// Ressourcenplanung löschen
		if(!$err)
		{
			$res=prj_get_resources($pid);
			$sor=sizeof($res);
//fxDebug($res, 'Alte Ressourcenzurondung ['.$sor.']');

			$pstbck	= $_POST;
			$_POST	= array();
			// ...Alte Zuordnung löschen
			if(is_array($res) && $sor)
			{
				$ekvk0=get_entry(0, 'dezimal');
				foreach($res as $r)
				{
					$opsid=(int)$r['personen_id'];

					$_POST['AZO'][$opsid]	= 1;
					$_POST['OEK'][$opsid]	= get_entry($r['ek_projekt'], 'dezimal');
					$_POST['OVK'][$opsid]	= get_entry($r['vk_projekt'], 'dezimal');
					$_POST['OAUFW'][$opsid]	= get_entry(round($r['prozent']*100.0, 2), 'dezimal');

					$_POST['EK'][$opsid]	= $ekvk0;
					$_POST['VK'][$opsid]	= $ekvk0;
					$_POST['AUFW'][$opsid]	= '';
					$_POST['OK'][$opsid]	= '';
				}
			}
			if(sizeof($_POST))
			{
				$rpid			= array(0=>$pid);
				$iau			= $pid;
				$iau_arr		= $rpid;
				$nosuccess_msg	= true;
				$_POST['MAZ']	= true;
//fxDebug('_POST', __FILE__.' ['.__LINE__.'] -- $_POST');

				require('ma_verfuegbarkeit_ini.inc');

				if($err)
					$cause='maplanung';
			}
			$_POST=$pstbck;
		}

		// Rechte (alte Tabelle)
		if(!$err)
		{
			$sql="SELECT rechte_referenzid FROM rechte_referenz WHERE id_referenz=".$pid." AND mandanten_id=".$mid." AND rechte_ref=37";
			$rex=(int)db_value($sql);
			if($rex)
			{
				$err=rechte_loeschen(37, $pid);
				if($err)
					$cause='rechte_referenz';
			}
		}

		// Projektfortschritt
		if(!$err)
		{
			$sql="SELECT id FROM projektfortschritt WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
			$fex=(int)db_value($sql);
			if($fex)
			{
				$sql="DELETE FROM projektfortschritt WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
				$err=db_x($sql);
//fxDebug($err, $sql);
				if($err)
					$cause='projektfortschritt';
			}
		}

		// Projekte
		if(!$err)
		{
			$sql="DELETE FROM projekte WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
			$err=db_x($sql);
//fxDebug($err, $sql);
			if($err)
				$cause='projekte';
		}

		// Zeitdaten
		if(!$err)
		{
			$sql="DELETE FROM zeitdaten WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
			$err=db_x($sql);
//fxDebug($err, $sql);
			if($err)
				$cause='zeitdaten';
		}

		// Budgetdaten verwalten
		if(!$err)
		{
			$err=update_budget($pid, false);
			if($err)
				$cause='budget_summe';
		}

		// PCA-Code verwalten
		if(!$err)
		{
			$npca=pca_code($pid);
			if($npca == '?')
			{
				$err=true; 
				$cause='budget_detail: pca';
			}
		}

		// Daten der übergeordneten Vorgänge korrigieren
		if(!$err && $his['aufwand_soll'])
		{
			$above_p=superior_vn($his['elter'], true);
			if($above_p && ($above_p != '-666'))
			{
				$sql="UPDATE projekte SET aufwand_soll=(aufwand_soll + (".$his['aufwand_soll'].")) WHERE projekt_id IN (".$above_p.") AND mandanten_id=".$mid;
				$err=db_x($sql);
//fxDebug($err, $sql);
				if($err)
					$cause='projekte: aufwand_soll';
			}
		}

		if($err)
		{
			if(strlen($cause))
			{
				// 1927: Fehler beim $var1 des Spezialprojektes "$var2"$var3
				error_msg(1927, false, '<b>'.$aaal[1].'</b> ['.$cause.']', '<b><font color=#0000AA>'.no_null(substr($parr['t|vorgangsnummer'], 1 ,-1)).'</font></b>&nbsp;&nbsp;<b>'.substr($parr['t|name_projekt'].'</b>', 1, -1), '!');
			}
			return true;
		}
		else
		{
			$GLOBALS['tvn_buffer'][$his['vorgangsnummer']]=$pid.'|'.$his['zeit_id'];
			ksort($GLOBALS['tvn_buffer']);
		}

		return false;
	}
}

/**
 * ???FUNCTION???
 */
function prj_userfields()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$GLOBALS['ufields']	= array();
	$GLOBALS['utable']	= '';
	$GLOBALS['unumber']	= 0;

	$sql  = "SELECT tm.nummer, m.name_maske FROM trans_maske tm, masken m";
	$sql .= " WHERE tm.nummer<0 AND tm.transakt_id=23 AND m.nummer=tm.nummer AND m.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$umt =db_values($sql);
	if(is_array($umt))
	{
		$GLOBALS['unumber']	= (int)$umt['nummer'];
		$GLOBALS['utable']	= strtolower(substr($umt['name_maske'], 1));

		$sql  = "SELECT mf.eingabefeld, f.feldname FROM maskenfelder mf, felder f";
		$sql .= " WHERE mf.nummer=".$GLOBALS['unumber']." AND mf.anzeigeart IN (6,7,8) AND f.id=mf.eingabefeld AND f.mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";
		$mskfldarr=db_values($sql, '*');
//fxDebug($mskfldarr, $sql);
		if(is_array($mskfldarr))
		{
			foreach($mskfldarr as $mskfld)
				$GLOBALS['ufields'][$mskfld['feldname']]=$mskfld['eingabefeld'];
		}
//fxDebug($GLOBALS['ufields'], '$ufields of table "'.$GLOBALS['utable'].'"');
	}
}

/**
 * safes the user form fields according to the current action
 *
 * @param various $values     - Mandatory parameter: ???PARAMETER???
 * @param various $action     - Mandatory parameter: ???PARAMETER???
 * @param various $process_id - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function safe_user_form_fields($values, $action, $process_id)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$user_form_fields=get_user_form_fields();
	if(fxIsArray($user_form_fields))
	{
		$proces_id=(int)$process_id;
		foreach($user_form_fields as $fields)
		{
			$field_string .= ','.$fields['feldname'];
			$value_string .= ','.$values[strtolower($fields['feldname'])];

			$update_string .= ','.$fields['feldname'].'='.$values[strtolower($fields['feldname'])];
		}

		// Check, if data exists
		$sql	= "";
		$tname	= substr($user_form_fields[0]['name_maske'], 1);
		$exists=(int)db_value("SELECT projekt_id FROM ".$tname." WHERE projekt_id=".$process_id." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);

		if(($action == FXP_CREATE) || ($action == FXP_CHANGE))
		{
			if($exists)
				$sql="UPDATE ".$tname." SET ".substr($update_string, 1)." WHERE projekt_id=".$process_id." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			else
				$sql="INSERT INTO ".$tname." (mandanten_id,projekt_id".$field_string.") VALUES (".$GLOBALS['fxpglobals']['client'].",".$process_id.$value_string.")";
		}
		else if(($action == FXP_DELETE) && $exists)
			$sql="DELETE FROM ".$tname." WHERE projekt_id=".$process_id." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];

		if(strlen($sql))
		{
//echo('<b>'.$action.'</b>: '.$sql.'<br />');
			return db_x($sql);
		}
	}
}

/**
 * ???FUNCTION???
 *
 * @param various $pid  - Mandatory parameter:                   ???PARAMETER???
 * @param various $ca   - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $incl - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function sub_change($pid, $ca, $incl=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pid=(int)$pid;
	if(!$pid || !is_array($ca))
		return true;

	$sql="SELECT mandanten_id, vorgangsnummer FROM projekte WHERE projekt_id=".$pid;
	$mva=db_values($sql);
	if(!is_array($mva))
		return true;

	$mid=(int)$mva['mandanten_id'];
	$vn =trim($mva['vorgangsnummer']);
	if(!$mid || !strlen($vn))
		return true;

	// Alle untergeordneten Vorgänge ermitteln
	$sql  = "SELECT projekt_id, vorgangsnummer, projektart, ansprechpartner, kunde, vertreter, projektleiter, projektmanager, kategorie_id";
	$sql .= " FROM projekte WHERE mandanten_id=".$mid." AND vorgangsnummer LIKE '".$vn."%'";
	$sql .= " ORDER BY vorgangsnummer";
	$pra =db_values($sql, '*');
//fxDebug($pra, $sql);
	if(!is_array($pra) || !sizeof($pra))
		return false;

	// Alle Vorgänge ermitteln, welche bereits abgerechnet wurden
	$inv =array();
	if($ca['ansprechpartner']['mode'])
	{
		$sql  = "SELECT projekt_id, (budget_pers_ext_abger+budget_mat_ext_abger) AS budget_ext_abger FROM budget_summe";
		$sql .= " WHERE projekt_id IN (SELECT projekt_id FROM projekte WHERE mandanten_id=".$mid." AND vorgangsnummer LIKE '".$vn."%')";
		$sql .= " AND (budget_pers_ext_abger>0 OR budget_mat_ext_abger>0)";
		$tmp =db_values($sql, '*');
//fxDebug($tmp, $sql);
		if(is_array($tmp))
		{
			foreach($tmp as $t)
				$inv[(int)$t['projekt_id']]=$t['budget_ext_abger'];
		}
//fxDebug($inv, $sql);
	}

	// Änderungs-SQLs ermitteln
	$sa=array();
	$za=array();
	$in=0;
	foreach($pra as $p)
	{
		$id=(int)$p['projekt_id'];
		if(!$id || (($id == $pid) && !$incl))
			continue;

		$sql="";
		foreach($ca as $cf => $ci)
		{
			$md=(int)$ci['mode'];
			$ov=(int)$ci['old'];
			$nv=(int)$ci['new'];
			$pv=(int)$p[$cf];
			if($md && ($ov != $nv) && isset($p[$cf]) && ($nv != $pv) && (($md == 2) || ($ov == $pv)))
			{
				// Ansprechpartner bzw. Kunden nicht ändern, falls bereits Rechnung zum Vorgang existiert
				if((($cf == 'ansprechpartner') || ($cf == 'kunde')) && $inv[$id])
				{
					if($cf == 'ansprechpartner')
						$in++;
				}
				else
				{
					$sql .= ",".$cf."=".$nv;
					// Falls der Kunde einer Aufgabe geändert wurde, müssen diese Änderungen auch bei den Zeitdaten durchgeführt werden
					if(($cf == 'kunde') && ($p['projektart'] == FXP_PRJ_TASK))
						$za[$id]="UPDATE zeitdaten SET zkunde=".$nv." WHERE projekt_id=".$id." AND mandanten_id=".$mid." AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.",".FXP_TE_TRAVELEXP.")";
				}
			}
		}
		if(strlen($sql))
			$sa[$id]=substr($sql, 1);
	}

	// Änderungs-SQLs ausführen
//fxDebug($sa, '$sa');
	if(sizeof($sa))
	{
		foreach($sa as $id => $values)
		{
			$sql="UPDATE projekte SET ".$values." WHERE projekt_id=".$id." AND mandanten_id=".$mid;
			$err=db_x($sql);
//fxDebug($err, $sql);
			if($err)
			{
				if(strpos($value, 'kategorie_id') != false)
					return 2777;	// 2777: Fehler beim Anpassen der [Projektkategorie] in den untergeordneten Vorgängen.
				return 2525;		// 2525: Fehler beim Anpassen der [Hauptverantwortlichen Personen] in den untergeordneten Vorgängen.
			}
		}
	}
//fxDebug($za, '$za');
	if(sizeof($za))
	{
		foreach($za as $id => $sql)
		{
			$err=db_x($sql);
//fxDebug($err, $sql);
			if($err)
				return 460;			// 460: Fehler beim Ändern des Datensatzes.
		}
	}

	// 2526: $var1 Ansprechpartner/Kunde(n) wurde(n) nicht angepasst, da zu den zugeordneten Vorgängen bereits Rechnungen vorhanden sind.
	if($in)
		error_msg(2526, false, $in);

	return false;
}

/**
 * ???FUNCTION???
 *
 * @param integer $client       - Optional parameter (default = 0):     ???PARAMETER???
 * @param integer $customer     - Optional parameter (default = 0):     ???PARAMETER???
 * @param boolean $ret_wildcard - Optional parameter (default = false): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_prefix($client=0, $customer=0, $ret_wildcard=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Platzhalter
	$prefix_wildcard=trim($GLOBALS['fxpglobals']['settings']['vnprefix']);
	if(!strlen($prefix_wildcard))
		return '';

	$client=(int)$client;
	if(!$client)
		$client=$GLOBALS['fxpglobals']['client'];

	$customer=(int)$customer;

	$prefix_nw='';
	$prefix_ps=0;
	$prefix_ad=0;
	$prefix_pa=array();
	$prefix_tp='';
	for($i=0; $i<strlen($prefix_wildcard); $i++)
	{
		$c=substr($prefix_wildcard, $i, 1);
		// Korrekte Zeichen
		if($prefix_tp == '\\')
		{
			$prefix_tp='';
			if($c != ' ')
			{
				if($ret_wildcard)
					$prefix_nw .= '*';
				else
					$prefix_nw .= $c;
				$prefix_ps++;
			}
		}
		else if($c == '\\')
			$prefix_tp=$c;
		else if(($c == 'Y') || ($c == 'M') || ($c == 'F') || ($c == 'G') || ($c == 'C') || ($c == 'D') || ($c == 'n') || ($c == 'm') || ($c == 'c'))
		{
			if($c == $prefix_tp)
			{
				if(($c != 'G') && ($c != 'D'))
				{
					$prefix_nw .= $c;
					$prefix_ps++;
					$prefix_pa[$prefix_ad]['len']++;
				}
			}
			else
			{
				if(is_array($prefix_pa))
					$prefix_ad++;

				if($c == 'G')
					$typ='_G';
				else if($c == 'D')
					$typ='_D';
				else
					$typ=$c;

				$prefix_pa[$prefix_ad]=array('pos'=>$prefix_ps, 'len'=>1, 'typ'=>$typ);

				if(substr($typ, 0, 1) == '_')
				{
					$prefix_nw .= ' '.$c.' ';
					$prefix_ps += 3;
				}
				else
				{
					$prefix_nw .= $c;
					$prefix_ps++;
				}

				$prefix_tp=$c;
			}
		}
		else if($c != ' ')
		{
			$prefix_nw .= $c;
			$prefix_tp='';
			$prefix_ps++;
		}
	}

	$prefix_wildcard=$prefix_nw;
//fxDebug($prefix_pa, $prefix_wildcard);

	if(!strlen($prefix_wildcard) || !sizeof($prefix_pa))
		return '';

	if($ret_wildcard)
		return $prefix_wildcard;


	// Mandanten-Initialen + Mandantenprojekte
	$client_array=get_prefix_client($client);
	$client_array['nr']++;

	// Auftraggeber-Initialen + Auftraggeberprojekte
	$customer_array=get_prefix_customer($customer);
	$customer_array['nr']++;

	// Ersetzung vornehmen
	$prefix=$prefix_wildcard;
	if(sizeof($prefix_pa))
	{
		$t=time();

		foreach($prefix_pa as $p)
		{
			$val	= false;
			$c		= $p['typ'];
			switch($c)
			{
				case 'Y':	// Jahr
					$val=substr(str_repeat('0', $p['len']).fx_date('Y', $t), -$p['len']);
				break;

				case 'M':	// Monat
					$val=substr(str_repeat('0', $p['len']).fx_date('m', $t), -$p['len']);
				break;

				case 'F':	// Identifikationsnummer Auftraggeber
					$val=substr($customer_array['val'].str_repeat(' ', $p['len']), 0, $p['len']);
				break;

				case 'C':	// Identifikationsnummer Mandant
					$val=substr($client_array['val'].str_repeat(' ', $p['len']), 0, $p['len']);
				break;

				case 'n':	// Fortlaufende Nummer Auftraggeber
					$val=substr(str_repeat('0', $p['len']).$customer_array['nr'], -$p['len']);
				break;

				case 'm':	// Fortlaufende Nummer Mandant
					$val=substr(str_repeat('0', $p['len']).$client_array['nr'], -$p['len']);
				break;

				case 'c':	// Mandantennummer
					$val=substr(str_repeat('0', $p['len']).$client, -$p['len']);
				break;
			}

			if($val !== false)
				$prefix=substr($prefix, 0, $p['pos']).$val.substr($prefix, $p['pos']+$p['len']);
		}
	}
	$prefix=strtr($prefix, array(' G '=> $customer_array['val'], ' D '=>$client_array['val'], ' '=>''));

//fxDebug($prefix, '$prefix');
	return $prefix;
}

/**
 * ???FUNCTION???
 *
 * @param integer $client - Optional parameter (default = 0): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_prefix_client($client=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$ret_array=array('val'=>'', 'nr'=>0);

	// Mandanten-Initialen
	$client=(int)$client;
	if(!$client)
		$client=$GLOBALS['fxpglobals']['client'];
	$sql="SELECT personen_initialen, pname FROM personen WHERE personentyp=274 AND mandanten_id=".$client;
	$tmp=db_values($sql);
//fxDebug($tmp, $sql);
	if(fxIsArray($tmp))
	{
		$ret_array['val']=trim($tmp['personen_initialen']);
		if(!strlen($ret_array['val']))
			$ret_array['val']=trim($tmp['pname']);
	}
	if(!strlen($ret_array['val']))
		$ret_array['val']='???';
	else
		$ret_array['val']=str_replace(' ', '', $ret_array['val']);

	// Mandantenprojekte
	if($client)
	{
		$sql="SELECT pzaehler FROM personen WHERE personentyp=274 AND mandanten_id=".$client;
		$tmp=(int)db_value($sql);
//fxDebug($tmp, $sql);
		if($tmp)
			$ret_array['nr']=max($ret_array['nr'], $tmp);

		$sql="SELECT COUNT(projekt_id) FROM projekte WHERE projektart=".FXP_PRJ_MAIN." AND mandanten_id=".$client;
		$tmp=(int)db_value($sql);
//fxDebug($tmp, $sql);
		if($tmp)
			$ret_array['nr']=max($ret_array['nr'], $tmp);
	}

	return $ret_array;
}

/**
 * ???FUNCTION???
 *
 * @param integer $customer - Optional parameter (default = 0): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_prefix_customer($customer=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$ret_array=array('val'=>'', 'nr'=>0);

	// Auftraggeber-Initialen
	$customer=(int)$customer;
	if($customer)
	{
		$sql="SELECT personen_initialen, pname FROM personen WHERE personentyp=276 AND personen_id=".$customer." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$tmp=db_values($sql);
//fxDebug($tmp, $sql);
		if(fxIsArray($tmp))
		{
			$ret_array['val']=trim($tmp['personen_initialen']);
			if(!strlen($ret_array['val']))
				$ret_array['val']=trim($tmp['pname']);
		}
	}
	if(!strlen($ret_array['val']))
		$ret_array['val']='???';
	else
		$ret_array['val']=str_replace(' ', '', $ret_array['val']);

	// Auftraggeberprojekte
	if($customer)
	{
		$sql="SELECT pzaehler FROM personen WHERE personentyp=276 AND personen_id=".$customer." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$tmp=(int)db_value($sql);
//fxDebug($tmp, $sql);
		if($tmp)
			$ret_array['nr']=max($ret_array['nr'],$tmp);

		$sql="SELECT COUNT(projekt_id) FROM projekte WHERE projektart=".FXP_PRJ_MAIN." AND kunde=".$customer." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$tmp=(int)db_value($sql);
//fxDebug($tmp, $sql);
		if($tmp)
			$ret_array['nr']=max($ret_array['nr'],$tmp);
	}

	return $ret_array;
}

/**
 * ???FUNCTION???
 *
 * @param various $project - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_prefix_project($project)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$p='';
	$project=(int)$project;

	$sql="SELECT mutterprojekt_id FROM projekte WHERE projekt_id=".$project." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$mp=(int)db_value($sql);
	if($mp <= 0)
		return $p;

	$sql="SELECT parameter_int, parameter_txt FROM projekte_parameter WHERE projekt_id=".$mp." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND parameter_typ=2299 AND parameter_var='TXT'";
	$dat=db_values($sql);
	if(!fxIsArray($dat))
		return $p;

	$i=(int)$dat['parameter_int'];
	$p=trim($dat['parameter_txt']);
	$l=strlen($p);
	if(!$l)
		return $p;

	// Sicherheitsüberprüfung für alte angelegte Prefixes, bei denen es noch kein separates Feld gab
	if(!$i)
	{
		$sql="SELECT vorgangsnummer,name_projekt FROM projekte WHERE (projekt_id=".$mp." OR elter=".$mp.") AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY vorgangsnummer";
		$cpa=db_values($sql,'*');
		if(fxIsArray($cpa))
		{
			foreach($cpa as $cp)
			{
				if(strlen($cp['name_projekt']) <= $l)
					$p='';
				else if(substr($cp['name_projekt'],0,$l) !== $p)
					$p='';

				if(!strlen($p))
					break;
			}
		}
	}
//echo('PREFIX='.$p.'<br />');

	return $p;
}

/**
 * ???FUNCTION???
 *
 * @param various $action - Mandatory parameter:                  ???PARAMETER???
 * @param various $mpid   - Mandatory parameter:                  ???PARAMETER???
 * @param various $cust   - Mandatory parameter:                  ???PARAMETER???
 * @param various $prefix - Mandatory parameter:                  ???PARAMETER???
 * @param boolean $sub    - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function handle_prefix($action, $mpid, $cust, $prefix, $sub=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mpid=(int)$mpid;
	if(!$mpid)
		return true;

	$cust=(int)$cust;
	$prefix=trim($prefix);
	$prefixl=strtolower($prefix);
	$pl=strlen($prefix);

	$no=0;
	$with_cust=false;
	$cust_no=0;
	$with_client=false;
	$client_no=0;
	$prefix_wildcard=trim($GLOBALS['fxpglobals']['settings']['vnprefix']);
	if(strlen($prefix_wildcard) && $pl)
	{
		$ns=-1;
		for($i=0; $i<$pl; $i++)
		{
			if(is_numeric(substr($prefix,$i,1)))
			{
				$ns=$i;
				break;
			}
		}
		if($ns >= 0)
		{
			$no=(int)substr($prefix,$ns);
			$v=true;
			for($i=0; $i<strlen($prefix_wildcard); $i++)
			{
				$c=substr($prefix_wildcard,$i,1);
				if($c == "\\")
					$v=false;
				else if(!$v)
					$v=true;
				else if($c == 'n')
					$with_cust=true;
				else if($c == 'm')
					$with_client=true;
			}
		}
	}
	if(($no > 0) && ($with_cust || $with_client))
	{
		if($with_cust && !$with_client)
			$cust_no=$no;
		else if(!$with_cust && $with_client)
			$client_no=$no;
	}

	// Bestand
	$sql="SELECT parameter_txt FROM projekte_parameter WHERE projekt_id=".$mpid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND parameter_typ=2299 AND parameter_var='TXT'";
	$oprefix=trim(db_value($sql));
	$oprefixl=strtolower($oprefix);
	$opl=strlen($oprefix);

	$sarr=array();
	// Prefix-Eintrag löschen?
	if((($action == FXP_DELETE) || !$pl) && $opl)
		$sarr[]="DELETE FROM projekte_parameter WHERE projekt_id=".$mpid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND parameter_typ=2299 AND parameter_var='TXT'";
	else if($pl)
	{
		// Prefix-Eintrag ändern?
		if($opl)
			$sarr[]="UPDATE projekte_parameter SET parameter_txt='".convert_string($prefix, 'todb')."', parameter_int=1 WHERE projekt_id=".$mpid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND parameter_typ=2299 AND parameter_var='TXT'";
		// Prefix-Eintrag neu anlegen?
		else
		{
			$nid=id_max('projekte_parameter', 'parameter_id');

			$sql  = "INSERT INTO projekte_parameter";
			$sql .= " (mandanten_id, parameter_id, projekt_id, parameter_typ, parameter_var, parameter_int, parameter_dec, parameter_txt, anlage_dtm, angelegt_von) ";
			$sql .= "VALUES";
			$sql .= " (".$GLOBALS['fxpglobals']['client'].", ".$nid.", ".$mpid.", 2299, 'TXT', 1, NULL, '".convert_string($prefix, 'todb')."', '".$GLOBALS['datetime']."', ".$GLOBALS['fxpglobals']['person'].")";

			$sarr[]=$sql;

			// Mandantenparameter
			$carr=get_prefix_client($GLOBALS['fxpglobals']['client']);
			$sarr[]="UPDATE personen SET pzaehler=".max($client_no,$carr['nr'])." WHERE personentyp=274 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];

			// Auftraggeberparameter
			if($cust)
			{
				$carr=get_prefix_customer($cust);
				$sarr[]="UPDATE personen SET pzaehler=".max($cust_no,$carr['nr'])." WHERE personentyp=276 AND personen_id=".$cust." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			}
		}
	}

	// Prefix bei allen untergeordneten Vorgängen anpassen
	if($sub && ($prefix !== $oprefix))
	{
		$sql="SELECT projekt_id, name_projekt FROM projekte WHERE mutterprojekt_id=".$mpid." AND projekt_id<>".$mpid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$spa=db_values($sql,'*');
//fxDebug($spa,$sql);
		if(fxIsArray($spa))
		{
			$p='';
			if($pl)
				$p=$prefix.' ';

			foreach($spa as $sp)
			{
				$on=$sp['name_projekt'];

				$nn=$on;
				if($opl && (trim(strtolower(substr($nn,0,$opl))) == $oprefixl))
					$nn=trim(substr($nn,$opl+1));
				if($pl && (trim(strtolower(substr($nn,0,$pl))) == $prefixl))
					$nn=trim(substr($nn,$pl+1));
				if(!strlen($nn))
					$nn='Dummy '.$GLOBALS['fxpglobals']['client'].'-'.$sp['projekt_id'];

				$nn=$p.$nn;

				if($nn !== $on)
					$sarr[]="UPDATE projekte SET name_projekt='".convert_string($nn, 'todb')."' WHERE projekt_id=".$sp['projekt_id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			}
		}
	}

	// Neue Werte in DB speichern
	$rerr=false;
	if(fxIsArray($sarr))
	{
		foreach($sarr as $sql)
		{
			$rerr=db_x($sql);
//fxDebug($rerr,$sql);
			if($rerr)
				break;
		}
	}

	return $rerr;
}

/**
 * ???FUNCTION???
 */
function getAllPrefixes()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$GLOBALS['mp_prefixes']=array();
	$sql="SELECT projekt_id, parameter_txt FROM projekte_parameter WHERE parameter_typ=2299 AND parameter_var='TXT' AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$tmp=db_values($sql,'*');
	if(fxIsArray($tmp))
	{
		foreach($tmp as $t)
			$GLOBALS['mp_prefixes'][(int)$t['projekt_id']]=trim($t['parameter_txt']).' ';
	}
}

/**
 * Alle Projektkategoriegruppen, Projektkategorien und Zuordnungen ermitteln
 *
 * @param boolean $assign_cat      - Optional parameter (default = true):  ???PARAMETER???
 * @param boolean $err_mode        - Optional parameter (default = false): ???PARAMETER???
 * @param boolean $with_unassigned - Optional parameter (default = false): ???PARAMETER???
 * @param boolean $with_processes  - Optional parameter (default = false): ???PARAMETER???
 */
function pk_getCategories($assign_cat=true, $err_mode=false, $with_unassigned=false, $with_processes=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Alle Projektkategoriegruppen und Projektkategorien aus der Wertetab-Tabelle auslesen
	pk_getCategoriesFromDB($with_unassigned);
	// Projektkategoriezuordnungen zu Gruppen ermitteln
	pk_getAssignedGroups($with_unassigned);
	// Projektkategoriezuordnungen zu Vorgängen ermitteln
	if($with_processes)
		pk_getAssignedProcesses($with_unassigned);

	if($assign_cat)
	{
		// Projektkategorien den Gruppen zuordnen
		pk_assignCategories2Groups($err_mode);
	}
}

/**
 * Alle Projektkategoriegruppen und Projektkategorien aus der Wertetab-Tabelle auslesen
 *
 * @param boolean $with_unassigned - Optional parameter (default = false): ???PARAMETER???
 */
function pk_getCategoriesFromDB($with_unassigned=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$treplace=array('"'=>'&quot;', '&'=>'&amp;');

	$sql  = "SELECT";
	$sql .= " mandanten_id, wertetabid, id_feld, position, tabwert ";
	$sql .= "FROM";
	$sql .= " wertetab ";
	$sql .= "WHERE";
	$sql .= " id_feld IN (1,10) AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].") AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND (archiv IS NULL OR archiv=0) ";
	$sql .= "ORDER BY";
	$sql .= " position, tabwert";
	$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql, 0);

	$GLOBALS['pga']=array();
	$GLOBALS['pca']=array();
	$GLOBALS['pg'] =array();
	$GLOBALS['pc'] =array();
	if(fxIsArray($tmp))
	{
		foreach($tmp as $t)
		{
			$cbt=pk_backgrounds(1, $t['tabwert']);
			$rval=strtr($t['tabwert'], $treplace);
			$tval=strtr($cbt['txt'], $treplace);

			$unique_id=substr('000000000'.(int)$t['position'], -9).'_'.str_replace(' ', '', strtolower($t['tabwert'])).'_'.abs((int)$t['wertetabid']);

			// Projektkategoriegruppe
			if($t['id_feld'] == 10)
			{
				$GLOBALS['pga'][(int)$t['wertetabid']]=$unique_id;
				$GLOBALS['pg'][$unique_id]=array('id'=>(int)$t['wertetabid'], 'mid'=>(int)$t['mandanten_id'], 'pos'=>(int)$t['position'], 'col'=>$cbt['col'], 'bg'=>$cbt['bg'], 'val'=>$tval, 'rval'=>$rval, 'prj'=>0, 'cat'=>array());
			}
			// Projektkategorie
			else
			{
				$GLOBALS['pca'][(int)$t['wertetabid']]=$unique_id;
				$GLOBALS['pc'][$unique_id]=array('id'=>(int)$t['wertetabid'], 'mid'=>(int)$t['mandanten_id'], 'pos'=>(int)$t['position'], 'col'=>$cbt['col'], 'bg'=>$cbt['bg'], 'val'=>$tval, 'rval'=>$rval, 'prj'=>0);
			}
		}
	}

	ksort($GLOBALS['pg']);
	ksort($GLOBALS['pc']);

	if($with_unassigned)
	{
		$unique_id='000000000_leer_00';
		$bg=pk_backgrounds(3, 0);
		$v597=meldung(597, false);	// Ohne Zuordnung
		$v794=meldung(597, false);	// -leer-

		// Projektkategoriegruppe
		$GLOBALS['pga'][0]=$unique_id;
		$GLOBALS['pg'][$unique_id]=array('id'=>0, 'mid'=>0, 'pos'=>0, 'col'=>0, 'bg'=>$bg, 'val'=>$v597, 'rval'=>$v597, 'prj'=>0, 'cat'=>array());	// Ohne Zuordnung
		// Projektkategorie
		$GLOBALS['pca'][0]=$unique_id;
		$GLOBALS['pc'][$unique_id]=array('id'=>0, 'mid'=>0, 'pos'=>0, 'col'=>0, 'bg'=>$bg, 'val'=>$v794, 'rval'=>$v794, 'prj'=>0);					// -leer-
	}
//fxDebug($GLOBALS['pg'], '$GLOBALS[\'pg\']', 0);
//fxDebug($GLOBALS['pc'], '$GLOBALS[\'pc\']', 0);
}

/**
 * Projektkategoriezuordnungen zu Gruppen ermitteln
 *
 * @param boolean $with_unassigned - Optional parameter (default = false): ???PARAMETER???
 */
function pk_getAssignedGroups($with_unassigned=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sql  = "SELECT";
	$sql .= " kategoriegruppe_id, kategorie_id ";
	$sql .= "FROM";
	$sql .= " kategoriezuordnung ";
	$sql .= "WHERE";
	$sql .= " mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";
	$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql, 0);

	$GLOBALS['pcl']=array();
	if(fxIsArray($tmp))
	{
		foreach($tmp as $t)
			$GLOBALS['pcl'][(int)$t['kategorie_id']]=(int)$t['kategoriegruppe_id'];
	}

	if($with_unassigned)
		$GLOBALS['pcl'][0]=0;
}

/**
 * Projektkategoriezuordnungen zu Vorgängen ermitteln
 *
 * @param boolean $with_unassigned - Optional parameter (default = false): ???PARAMETER???
 */
function pk_getAssignedProcesses($with_unassigned=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(fxIsArray($GLOBALS['pca']))
	{
		$sql  = "SELECT";
		$sql .= " kategorie_id, COUNT(projekt_id) AS anz ";
		$sql .= "FROM";
		$sql .= " projekte ";
		$sql .= "WHERE";
		$sql .= " mandanten_id=".$GLOBALS['fxpglobals']['client']." AND projekt_id>0 AND (kategorie_id IS NOT NULL AND kategorie_id>0) ";
		$sql .= "GROUP BY";
		$sql .= " kategorie_id";
		$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql, 0);
		if(is_array($tmp))
		{
			foreach($tmp as $t)
			{
				$cid=(int)$t['kategorie_id'];
				if(isset($GLOBALS['pga'][$cid]))
					$GLOBALS['pg'][$GLOBALS['pga'][$cid]]['prj']=(int)$t['anz'];
				if(isset($GLOBALS['pca'][$cid]))
					$GLOBALS['pc'][$GLOBALS['pca'][$cid]]['prj']=(int)$t['anz'];
			}
		}

		if($with_unassigned)
		{
			$sql  = "SELECT";
			$sql .= " COUNT(projekt_id) AS anz ";
			$sql .= "FROM";
			$sql .= " projekte ";
			$sql .= "WHERE";
			$sql .= " mandanten_id=".$GLOBALS['fxpglobals']['client']." AND projekt_id=0 OR kategorie_id IS NULL OR kategorie_id=0";
			$anz=(int)db_value($sql);
//fxDebug($anz, $sql, 0);
			$GLOBALS['pc'][$GLOBALS['pca'][0]]['prj']=$anz;
		}
	}
}

/**
 * Projektkategorien den Gruppen zuordnen
 *
 * @param boolean $err_mode - Optional parameter (default = false): ???PARAMETER???
 */
function pk_assignCategories2Groups($err_mode=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($err_mode)
	{
		$GLOBALS['pg']=$GLOBALS['sg'];

		if(fxIsArray($GLOBALS['sc']))
		{
			foreach($GLOBALS['sc'] as $cunique_id => $c)
			{
				$cid=$c['id'];
				if($cid < 0)
					$cid='n'.$c['cnt'];
				$did=2779;	// Ohne Zuordnung
				if(fxIsArray($GLOBALS['scl']) && isset($GLOBALS['scl'][$cid]))
					$did=$GLOBALS['scl'][$cid];
				if(!fxIsArray($GLOBALS['sga']) || !isset($GLOBALS['sga'][$did]))
					$did=2779;

				if(fxIsArray($GLOBALS['sga']) && isset($GLOBALS['sga'][$did]))
				{
					$gunique_id=$GLOBALS['sga'][$did];
					$GLOBALS['pg'][$gunique_id]['cat'][$cunique_id]=$c;
				}
			}
		}
	}
	else
	{
		if(fxIsArray($GLOBALS['pc']))
		{
			foreach($GLOBALS['pc'] as $cunique_id => $c)
			{
				$cid=(int)$c['id'];
				$did=2779;	// Ohne Zuordnung
				if(fxIsArray($GLOBALS['pcl']) && isset($GLOBALS['pcl'][$cid]))
					$did=$GLOBALS['pcl'][$cid];
				if(!fxIsArray($GLOBALS['pga']) || !isset($GLOBALS['pga'][$did]))
					$did=2779;

				if(fxIsArray($GLOBALS['pga']) && isset($GLOBALS['pga'][$did]))
				{
					$gunique_id=$GLOBALS['pga'][$did];
					$GLOBALS['pg'][$gunique_id]['cat'][$cunique_id]=$c;
				}
			}
		}
	}
}

/**
 * ???FUNCTION???
 *
 * @param integer $mode - Optional parameter (default = 0):  ???PARAMETER???
 * @param string  $v1   - Optional parameter (default = ''): ???PARAMETER???
 * @param string  $v2   - Optional parameter (default = ''): ???PARAMETER???
 * @param integer $l    - Optional parameter (default = 0):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function pk_backgrounds($mode=0, $v1='', $v2='', $l=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mode=(int)$mode;

	$l=(int)$l;
	if(!$l)
		$l=$GLOBALS['fxpglobals']['lang'];

	if($l == 1)
		$pcn='Standard|Grau|Blau|Grün|Zyan|Gelb|Rot|Orange|Braun|Purpur';
	else
		$pcn='Standard|Grey|Blue|Green|Cyan|Yellow|Red|Orange|Brown|Purple';
	$pcc='url('.$GLOBALS['gfxpath'].'pcb.png) no-repeat top left|dcdcdc|7cb0ca|7ccab0|7ce6e6|dcdc88|ea7c89|edbc9b|c8b199|c77ce0';

	if($mode <= 0)
	{
		echo('<div id="prjcat_bgnames" style="display:none;">'.$pcn.'</div>');
		echo('<div id="prjcat_bgcolors" style="display:none;">'.$pcc.'</div>');
		return;
	}

	else if($mode == 1)
	{
		$pca=explode('|', $pcc);
		$s=false;
		$c='';
		$b=$pca[0];
		$t=$v1;
		$p=strpos($t,'<bg#');
		if($p !== false)
		{
			$s=strtolower(substr($t,$p+4,6));
			$t=trim(substr($t,0,$p).substr($t,$p+11));
			foreach($pca as $pc => $pb)
			{
				if($s === $pb)
				{
					$s=true;
					$c=$pc;
					$b='#'.$pb;
					break;
				}
			}
		}
		return array('set'=>$s, 'col'=>$c, 'bg'=>$b, 'txt'=>$t);
	}

	else if($mode == 2)
	{
		$pna=explode('|', $pcn);
		$pca=explode('|', $pcc);

		if($l == 1)
			$no='Ohne Zuordnung <i>(= Standardfarbe)</i>';
		else
			$no='Unassigned <i>(= Standard Color)</i>';

		echo('	<tr '.fxf_jsFunction('prjcatSetBG', $v1, $v2, -1).' style="cursor:pointer;">');
		echo('		<td align="center" width="16" nowrap style="border:1px solid #888888;border-radius:6px;">&nbsp;&nbsp;&nbsp;</td>');
		echo('		<td>&nbsp;</td>');
		echo('		<td align="left" nowrap style="cursor:pointer;background:#f8f8f8;">&nbsp;&nbsp;<font class="darkgrey">'.$no.'</font></td>');
		echo('	</tr>');
		foreach($pca as $pc => $bg)
		{
			if($pc > 0)
			{
				echo('	<tr '.fxf_jsFunction('prjcatSetBG', $v1, $v2, $pc).' style="cursor:pointer;">');
				echo('		<td align="center" nowrap style="border:1px solid #888888;border-radius:6px;">&nbsp;'.$pc.'&nbsp;</td>');
				echo('		<td>&nbsp;</td>');
				echo('		<td align="left" nowrap style="cursor:pointer;background:url('.$GLOBALS['gfxpath'].'bg_tsk_'.$pc.'.png);">&nbsp;&nbsp;'.$pna[$pc].'</td>');
				echo('	</tr>');
			}
		}
		return;
	}

	else if($mode == 3)
	{
		$pca=explode('|', $pcc);
		$v=(int)$v1;
		if($v <= 0)
			return $pca[0];
		if($v >= sizeof($pca))
			$v=sizeof($pca)-1;
		return '#'.$pca[$v];
	}
}

/**
 * ???FUNCTION???
 *
 * @param various $kid    - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $as_sql - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function pk_ids($kid, $as_sql=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$kid=trim($kid);
	$ke=explode(',',$kid);
	$ki='';
	$st=false;
	foreach($ke as $k)
	{
		$i=(int)$k;
		$ki .= ','.$i;
		if($i == 2779)	// Standard group
			$st=true;
	}
	$ki=substr($ki,1);

	$sql  = "SELECT wertetabid FROM wertetab WHERE id_feld IN (1,10) AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";
	$sql .= " AND (wertetabid IN (".$ki.") OR (wertetabid IN (SELECT kategorie_id FROM kategoriezuordnung WHERE kategoriegruppe_id IN (".$ki.") AND mandanten_id=".$GLOBALS['fxpglobals']['client']."))";
	if($st)
		$sql .= " OR (id_feld=1 AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].") AND wertetabid NOT IN (SELECT kategorie_id FROM kategoriezuordnung WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']."))";
	$sql .= ")";

	if($as_sql)
		return $sql;

	$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql, 0);
	if(fxIsArray($tmp))
	{
		$csv='';
		foreach($tmp as $t)
			$csv .= ','.$t['wertetabid'];
		$csv=substr($csv,1);
	}
	else
		$csv='0';
//fxDebug($csv,'$csv');

	return $csv;
}

/**
 * ???FUNCTION???
 *
 * @param various $top_pid   - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $criterias - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $plan      - Optional parameter (default = true):   ???PARAMETER???
 * @param boolean $prj       - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function netplan($top_pid, $criterias=false, $plan=true, $prj=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Projekt-ID der obersten Ebene
	$top_pid=(int)$top_pid;
	if($top_pid <= 0)
		return false;

	// Nur mit Solldaten arbeiten
	if($plan)
		$plan=true;
	else
		$plan=false;

	// Puffertage und Krit. Pfad
	$buffer=0;
	$with_cp=true;
	$align_workdays=true;
	$sdate='';
	$edate='';
	if(is_array($criterias) && sizeof($criterias))
	{
		// ...Zeitlicher zu kalkulierender Mindestabstand zwischen zwei Aufgaben
		if(isset($criterias['v_buffer']) && $criterias['v_buffer'])
			$buffer=max(0, (int)$criterias['v_buffer']);
		// ...Krit. Pfad anzeigen?
		if(!isset($criterias['v_cpath']) || !$criterias['v_cpath'])
			$with_cp=false;
		// ...Auf Wochentage ausrichten?
		if(!isset($criterias['v_wd']) || !$criterias['v_wd'])
			$align_workdays=false;
		// ...Gepl. Startdatum
		if(isset($criterias['sd']) && strlen($criterias['sd']))
			$sdate=substr($criterias['sd'], 0, 8);
		// ...Gepl. Enddatum
		if(isset($criterias['ed']) && strlen($criterias['ed']))
			$edate=substr($criterias['ed'], 0, 8);
	}
	$osdate=$sdate;
	$oedate=$edate;
//echo('$sdate='.$sdate.', $edate='.$edate.' -- $align_workdays='.$align_workdays.'<br />');

	// Mandanten-ID und Vorgangsnummer des übergebenen Projektes ermitteln
	$mid= $GLOBALS['fxpglobals']['client'];
	$sql="SELECT vorgangsnummer FROM projekte WHERE projekt_id=".$top_pid." AND mandanten_id=".$mid;
	$tmp=db_values($sql);
//fxDebug($tmp, $sql);
	$top_vn	= '';
	if(is_array($tmp))
		$top_vn=$tmp['vorgangsnummer'];
	if(!strlen($top_vn))
		return false;

	// Projektstrukturinfos ermitteln
	if(!is_array($prj) || !sizeof($prj))
	{
		// Projektstrukturinfos ermitteln
		$sql  = "SELECT p.projekt_id, p.elter, p.vorgangsnummer, p.name_projekt, p.projektart, p.projekt_status, p.prioritaet, p.abhaengigkeit,";
		$sql .= " p.aufwand_soll, p.aufwand_ist, p.deadline, p.kunde, p.projektmanager, p.projektleiter, p.vertreter,";
		$sql .= " z.soll_beginn, z.soll_ende, z.ist_beginn, z.ist_ende";
		$sql .= " FROM projekte p, zeitdaten z";
		$sql .= " WHERE p.vorgangsnummer LIKE '".$top_vn."%' AND p.mandanten_id=".$mid." AND p.projektart<=".FXP_PRJ_TASK." AND z.projekt_id=p.projekt_id AND z.zeitart=102 AND z.mandanten_id=".$mid;
		// ...Einschränkungskriterien
		if(is_array($criterias) && sizeof($criterias))
		{
			$tsql='';
			// ...Hauptverantwortliche
			if(isset($criterias['pl']) && ($criterias['pl'] > 0))
				$tsql .= " AND (p.projektleiter=".$criterias['pl']." OR p.vertreter=".$criterias['pl'].")";

			$bsql='';
			// ...Hauptverantwortliche
			if(isset($criterias['cust']) && ($criterias['cust'] > 0))
				$bsql .= " AND p.kunde=".$criterias['cust'];
			if(isset($criterias['pm']) && ($criterias['pm'] > 0))
				$bsql .= " AND (p.projektmanager IS NULL OR p.projektmanager=0 OR p.projektmanager=".$criterias['pm'].")";

			if(strlen($tsql) || strlen($bsql))
				$sql .= " AND ((p.projektart<".FXP_PRJ_TASK.$bsql.") OR (p.projektart=".FXP_PRJ_TASK.$tsql.$bsql."))";
		}
		$sql .= " ORDER BY p.vorgangsnummer";
		$prj =db_values($sql, '*');
	}
//fxDebug($prj, $sql);
	if(!is_array($prj))
		return false;

	$today=substr($GLOBALS['datetime'],0,8);	// Heutiges Datum

	// Minimum-Startdatum von Aufgaben ermitteln
	if(!strlen($sdate))
	{
		$sdate='99999999';
		foreach($prj as $idx => $dat)
		{
			if($dat['projektart'] == FXP_PRJ_TASK)
				$sdate=min($sdate, substr($dat['soll_beginn'], 0, 8));
		}
	}

	// Vorgangsnummern-Array
	$vn_arr=array();
	foreach($prj as $idx => $dat)
		$vn_arr[$dat['vorgangsnummer']]=$idx;
//fxDebug($vn_arr, '$vn_arr');

	// Array zum Speichern der Netzplaninfos
	$calc=2;
	while($calc > 0)
	{
//echo('<hr />$calc: '.$calc.' -- $sdate='.$sdate.'<hr />');
		$calc--;

		$prj_info_arr=array(
			'var' => array(
				'min_sdate'	=> '99999999',
				'max_edate'	=> '00000000',
				'daydiff'	=> 0,
				'max_fe'	=> 0,
				'cp_vn'		=> '',
				'cp_column'	=> 0,
				'columns'	=> 0,
				'lines'		=> 0
			)
		);

		// Netzplan: Vorwärtsplanung
		$cp_vp='';
		foreach($vn_arr as $vn => $pkey)
		{
			$pdata=$prj[$pkey];

			$ptype=1;
			if($pdata['projektart'] < FXP_PRJ_TASK)
				$ptype=0;

			$pparent='';
			if(strlen($vn) > 4)
				$pparent=substr($vn, 0, -5);

			$prj_info_arr[$vn]=array
			(
				'nr'		=> no_null($vn),
				'name'		=> $pdata['name_projekt'],
				'type'		=> $ptype,
				'ptype'		=> (int)$pdata['projektart'],
				'stat'		=> (int)$pdata['projekt_status'],
				'prio'		=> (int)$pdata['prioritaet'],

				'parent'	=> $pparent,
				'pre'		=> $pparent,
				'nxt'		=> '',
				'column'	=> 0,
				'line'		=> '',
				'cp'		=> false,

				'len'		=> 0,

				'fb'		=> 0,
				'fe'		=> 0,
				'sb'		=> 0,
				'se'		=> 0,

				'fbd'		=> '',
				'fed'		=> '',
				'sbd'		=> '',
				'sed'		=> '',

				'gp'		=> 0,
				'fp'		=> 0,

				'ps'		=> substr($pdata['soll_beginn'], 0, 8),
				'pe'		=> substr($pdata['soll_ende'], 0, 8),
				'as'		=> substr($pdata['ist_beginn'], 0, 8),
				'ae'		=> substr($pdata['ist_ende'], 0, 8),

				'disp'		=> array('border-style' => "outset", 'border-color' => "aaaaaa", 'border-width' => 1, 'image' => "")
			);

			if($vn != $top_vn)
			{
				$prj_info_arr[$vn]['column']=$prj_info_arr[$pparent]['column']+1;

				// Projekt
				if(!$ptype)
				{
					$prj_info_arr[$vn]['fb']=$prj_info_arr[$pparent]['fb'];
					$prj_info_arr[$vn]['fe']=$prj_info_arr[$pparent]['fe'];
					$prj_info_arr[$vn]['sb']=$prj_info_arr[$pparent]['sb'];
					$prj_info_arr[$vn]['se']=$prj_info_arr[$pparent]['se'];
				}
				// Aufgabe
				else
				{
					$len_d=ceil($pdata['aufwand_soll']/(3600*$GLOBALS['fxpglobals']['settings']['mtag']));
					$prj_info_arr[$vn]['len']=$len_d;

					// Folgen
					// ...ohne
					$prj_info_arr[$vn]['fb']=$prj_info_arr[$pparent]['fe'];
					// ...Normalfolge: Ende-Anfang (EA-Beziehung)
					$pvn='';
					foreach($prj_info_arr as $cvn => $cdata)
					{
						if(($cvn != 'var') && ($vn != $cvn) && ($pparent == $cdata['parent']) && $cdata['type'])
							$pvn=$cvn;
					}
					if(strlen($pvn))
					{
						$prj_info_arr[$pvn]['nxt']=$vn;
						$prj_info_arr[$vn]['pre']=$pvn;

						$prj_info_arr[$vn]['column']=$prj_info_arr[$pvn]['column']+1;

						$prj_info_arr[$vn]['fb']=$prj_info_arr[$pvn]['fe'];
					}
					// Beginn: Ausrichten auf Wochentag
					if($align_workdays)
					{
						$tsd=time_add($sdate, $prj_info_arr[$vn]['fb'], 'D');
						$ted=time_add($tsd, 14+(int)(($buffer)*7/5), 'D');
						$holidays=get_feiertage($tsd, $ted);
						$wday=getWeekday($tsd);
						// Starttag muss an einem Wochentag liegen
						while(!$wday || ($wday>5) || (is_array($holidays) && isset($holidays[$tsd]) && ($holdidays[$tsd]['typ'] > 0.0)))
						{
							$prj_info_arr[$vn]['fb']++;
							$tsd=time_add($sdate, $prj_info_arr[$vn]['fb'], 'D');
							$wday=getWeekday($tsd);
						}
						// Evtl. x Pufferwochentage hinzufügen
						if($prj_info_arr[$prj_info_arr[$vn]['pre']]['fe'])
						{
							$wcnt=$buffer;
							$prj_info_arr[$vn]['fb']++;
							$tsd=time_add($sdate, $prj_info_arr[$vn]['fb'], 'D');
							$wday=getWeekday($tsd);
							while($wcnt || !$wday || ($wday>5) || (is_array($holidays) && isset($holidays[$tsd]) && ($holidays[$tsd]['typ'] > 0.0)))
							{
								if($wday && ($wday<6) && (!is_array($holidays) || !isset($holidays[$tsd]) || ($holidays[$tsd]['typ'] == 0.0)))
								{
									$wcnt--;
								}

								$prj_info_arr[$vn]['fb']++;
								$tsd=time_add($sdate, $prj_info_arr[$vn]['fb'], 'D');
								$wday=getWeekday($tsd);
							}
						}
					}
					else if($prj_info_arr[$vn]['fb'])
						$prj_info_arr[$vn]['fb'] += $buffer+1;

					// Ende: Ausrichten auf Wochentag
					if($align_workdays)
					{
						$prj_info_arr[$vn]['fe']=$prj_info_arr[$vn]['fb'];
						$tsd=time_add($sdate, $prj_info_arr[$vn]['fe'], 'D');
						$ted=time_add($tsd, 14+(int)(($len_d)*7/5), 'D');
						$holidays=get_feiertage($tsd, $ted);
						$wday=getWeekday($tsd);
						// x Wochentage hinzufügen
						$wcnt=$len_d;
						while($wcnt || !$wday || ($wday>5) || (is_array($holidays) && isset($holidays[$tsd]) && ($holidays[$tsd]['typ'] > 0.0)))
						{
							if($wday && ($wday<6) && (!is_array($holidays) || !isset($holidays[$tsd]) || ($holidays[$tsd]['typ'] == 0.0)))
							{
								$wcnt--;
								if(!$wcnt)
									break;
							}

							$prj_info_arr[$vn]['fe']++;
							$tsd=time_add($sdate, $prj_info_arr[$vn]['fe'], 'D');
							$wday=getWeekday($tsd);
						}
					}
					else
						$prj_info_arr[$vn]['fe']=max(0, $prj_info_arr[$vn]['fb']+$len_d-1);

					$prj_info_arr[$vn]['sb']=$prj_info_arr[$vn]['fb'];
					$prj_info_arr[$vn]['se']=$prj_info_arr[$vn]['fe'];
				}
			}

			if(($plan || ($pdata['projekt_status'] == FXP_PS_PLANNED) || ($pdata['projekt_status'] == FXP_PS_ACTIVE)) && ($prj_info_arr[$vn]['fe'] >= $prj_info_arr['var']['max_fe']))
			{
				if(($prj_info_arr[$vn]['fe'] > $prj_info_arr['var']['max_fe']) || ($prj_info_arr[$vn]['column'] >= $prj_info_arr['var']['cp_column']))
				{
					$prj_info_arr['var']['max_fe']=$prj_info_arr[$vn]['fe'];
					$prj_info_arr['var']['cp_column']=$prj_info_arr[$vn]['column'];
					$prj_info_arr['var']['cp_vn']=$vn;
				}
			}
		}
//echo('max_fe='.$prj_info_arr['var']['max_fe'].', cp_column='.$prj_info_arr['var']['cp_column'].', cp_vn='.$prj_info_arr['var']['cp_vn'].'<br />'.$nl);

		// Falls Enddatum angegeben, aber kein Startdatum, dann dieses anhand des längsten Pfades ermitteln...
		if($calc && strlen($oedate) && !strlen($osdate))
		{
			$vn			= $prj_info_arr['var']['cp_vn'];
			$sdate		= $oedate;
			$holidays	= get_feiertage(time_add($oedate, -(int)(($prj_info_arr['var']['max_fe']*7)/5), 'D'), $oedate);
//fxDebug($holidays, time_add($oedate, -(int)(($prj_info_arr['var']['max_fe']*7)/5), 'D').' - '.$oedate);

			while(strlen($vn) && $prj_info_arr[$vn]['type'])
			{
				// Puffer
				if($prj_info_arr[$vn]['nxt'])
				{
					$sdate=time_add($sdate, -1, 'D');
					// x Puffertage abziehen
					$wcnt=$buffer;
					if($wcnt)
					{
						if($align_workdays)
						{
							$wday=getWeekday($sdate);
							while($wcnt || !$wday || ($wday>5) || (is_array($holidays) && isset($holidays[$sdate]) && ($holidays[$sdate]['typ'] > 0.0)))
							{
								if($wday && ($wday<6) && (!is_array($holidays) || !isset($holidays[$sdate]) || ($holidays[$sdate]['typ'] == 0.0)))
								{
									$wcnt--;
									if(!$wcnt)
										break;
								}
								$sdate=time_add($sdate, -1, 'D');
								$wday=getWeekday($sdate);
							}
							$sdate=time_add($sdate, -1, 'D');
						}
						else
							$sdate=time_add($sdate, -$wcnt, 'D');
					}
				}

				// Beginn: Ausrichten auf Wochentag
				if($align_workdays)
				{
					$wday=getWeekday($sdate);
					// x Wochentage abziehen
					$wcnt=$prj_info_arr[$vn]['len'];
					while($wcnt || !$wday || ($wday>5) || (is_array($holidays) && isset($holidays[$sdate]) && ($holidays[$sdate]['typ'] > 0.0)))
					{
						if($wday && ($wday<6) && (!is_array($holidays) || !isset($holidays[$sdate]) || ($holidays[$sdate]['typ'] == 0.0)))
						{
							$wcnt--;
							if(!$wcnt)
								break;
						}
						$sdate=time_add($sdate, -1, 'D');
						$wday=getWeekday($sdate);
					}
				}
				else
					$sdate=time_add($sdate, 1-max(1, $prj_info_arr[$vn]['len']), 'D');

				$vn=$prj_info_arr[$vn]['pre'];
			}
//echo('<b>Recalculate start date ('.$sdate.')!</b><br />');
		}
		else
			$calc=0;
	}

	// Zeilen eintragen
	foreach($prj_info_arr as $vn => $data)
	{
		if(($vn != 'var') && $data['parent'] && !strlen($data['nxt']))
		{
			$prj_info_arr['var']['columns']=max($prj_info_arr['var']['columns'], $data['column']+1);
			$lvn=$vn;
			while(strlen($lvn) && !strlen($prj_info_arr[$lvn]['line']))
			{
				$prj_info_arr[$lvn]['line']=$prj_info_arr['var']['lines'];
				$lvn=$prj_info_arr[$lvn]['pre'];
			}
			// Gibt es einen Nachfolger?
			$nxt=false;
			foreach($prj_info_arr as $nvn => $ndata)
			{
				if($ndata['parent'] == $vn)
				{
					$nxt=true;
					break;
				}
			}
			if(!$nxt)
				$prj_info_arr['var']['lines']++;
		}
	}
//echo('<hr />lines='.$prj_info_arr['var']['lines'].', columns='.$prj_info_arr['var']['columns'].'<br />'.$nl);

	// Kritischen Pfad eintragen
	if($with_cp)
	{
		$vn=$prj_info_arr['var']['cp_vn'];
		while(strlen($vn))
		{
			$prj_info_arr[$vn]['cp']=true;
			$vn=$prj_info_arr[$vn]['pre'];
		}
	}

	// Maximum-Enddatum von Aufgaben ermitteln
	if(!strlen($oedate))
	{
		if(strlen($osdate))
			$edate=time_add($sdate, $prj_info_arr['var']['max_fe'], 'D');
		else
		{
			$edate='00000000';
			foreach($prj as $idx => $dat)
			{
				if($dat['projektart'] == FXP_PRJ_TASK)
					$edate=max($edate, substr($dat['soll_ende'], 0, 8));
			}
		}
	}
	$prj_info_arr['var']['min_sdate']=$sdate;
	$prj_info_arr['var']['max_edate']=$edate;

	// Geplante Tagesdifferenz anhand Start- und Enddatum (für Rückwartsplanung)
	$prj_info_arr['var']['daydiff']=fxp_date_diff($prj_info_arr['var']['min_sdate'], $prj_info_arr['var']['max_edate']);
//echo('min_sdate='.$prj_info_arr['var']['min_sdate'].', max_edate='.$prj_info_arr['var']['max_edate'].' =&gt; daydiff='.$prj_info_arr['var']['daydiff'].'<br />'.$nl);

	// Netzplan: Rückwärtsplanung nur effektiv, falls geplante Projektdauer größer ist als die in der Vorwärtsplanung ermittelte Dauer
	$prj_info_arr['var']['daydiff']=max($prj_info_arr['var']['max_fe'], $prj_info_arr['var']['daydiff']);
	$holidays=get_feiertage($prj_info_arr['var']['min_sdate'], $prj_info_arr['var']['max_edate']);
	foreach($prj_info_arr as $vn => $data)
	{
		if(($vn != 'var') && $data['type'] && !strlen($data['nxt']))
		{
			$prj_info_arr[$vn]['se']=$prj_info_arr['var']['daydiff'];
			// Beginn: Ausrichten auf Wochentag
			if($align_workdays)
			{
				$prj_info_arr[$vn]['sb']=$prj_info_arr[$vn]['se'];
				$tsd=time_add($prj_info_arr['var']['min_sdate'], $prj_info_arr[$vn]['sb'], 'D');
				$wday=getWeekday($tsd);
				// x Wochentage abziehen
				$wcnt=$prj_info_arr[$vn]['len'];
				while($wcnt || !$wday || ($wday>5) || (is_array($holidays) && isset($holidays[$tsd]) && ($holidays[$tsd]['typ'] > 0.0)))
				{
					if($wday && ($wday<6) && (!is_array($holidays) || !isset($holidays[$tsd]) || ($holidays[$tsd]['typ'] == 0.0)))
					{
						$wcnt--;
						if(!$wcnt)
							break;
					}

					$prj_info_arr[$vn]['sb']--;
					$tsd=time_add($prj_info_arr['var']['min_sdate'], $prj_info_arr[$vn]['sb'], 'D');
					$wday=getWeekday($tsd);
				}
			}
			else
				$prj_info_arr[$vn]['sb']=$prj_info_arr[$vn]['se']+1-max(1, $prj_info_arr[$vn]['len']);
			$prj_info_arr[$vn]['gp']=$prj_info_arr[$vn]['sb']-$prj_info_arr[$vn]['fb'];

			$avn=$vn;
			while(true)
			{
				$pvn=$prj_info_arr[$avn]['pre'];
				if(!strlen($pvn))
					break;

				if($prj_info_arr[$pvn]['len'])
				{
					$prj_info_arr[$pvn]['se']=$prj_info_arr[$avn]['sb']-1;
					// Ende: Ausrichten auf Wochentag
					if($align_workdays)
					{
						$tsd=time_add($prj_info_arr['var']['min_sdate'], $prj_info_arr[$pvn]['se'], 'D');
						$wday=getWeekday($tsd);
						while(!$wday || ($wday>5) || (is_array($holidays) && isset($holidays[$tsd]) && ($holidays[$tsd]['typ'] > 0.0)))
						{
							$prj_info_arr[$pvn]['se']--;
							$tsd=time_add($prj_info_arr['var']['min_sdate'], $prj_info_arr[$pvn]['se'], 'D');
							$wday=getWeekday($tsd);
						}

						// Beginn: Ausrichten auf Wochentag
						$prj_info_arr[$pvn]['sb']=$prj_info_arr[$pvn]['se'];
						$tsd=time_add($prj_info_arr['var']['min_sdate'], $prj_info_arr[$pvn]['sb'], 'D');
						$wday=getWeekday($tsd);
						// x Wochentage abziehen
						$wcnt=$prj_info_arr[$pvn]['len'];
						while($wcnt || !$wday || ($wday>5) || (is_array($holidays) && isset($holidays[$tsd]) && ($holidays[$tsd]['typ'] > 0.0)))
						{
							if($wday && ($wday<6) && (!is_array($holidays) || !isset($holidays[$tsd]) || ($holidays[$tsd]['typ'] == 0.0)))
							{
								$wcnt--;
								if(!$wcnt)
									break;
							}

							$prj_info_arr[$pvn]['sb']--;
							$tsd=time_add($prj_info_arr['var']['min_sdate'], $prj_info_arr[$pvn]['sb'], 'D');
							$wday=getWeekday($tsd);
						}
					}
					else
						$prj_info_arr[$pvn]['sb']=$prj_info_arr[$pvn]['se']+1-max(1, $prj_info_arr[$pvn]['len']);

					$prj_info_arr[$pvn]['gp']=$prj_info_arr[$pvn]['sb']-$prj_info_arr[$pvn]['fb'];
					$prj_info_arr[$pvn]['fp']=max(0, $prj_info_arr[$avn]['fb']-$prj_info_arr[$pvn]['fe']-1);
				}

				$avn=$pvn;
			}
		}
	}

	// Daten für Projekte nachtragen
	foreach($prj_info_arr as $vn => $data)
	{
		if(($vn != 'var') && $data['type'] && !strlen($data['nxt']))
		{
			$fe=$prj_info_arr[$vn]['fe'];
			$sb=$prj_info_arr[$vn]['sb'];
			$se=$prj_info_arr[$vn]['se'];

			$avn=$vn;
			while(true)
			{
				if(!$prj_info_arr[$avn]['type'])
				{
					if(!$prj_info_arr[$avn]['fe'] || ($prj_info_arr[$avn]['fe'] < $fe))
						$prj_info_arr[$avn]['fe']=$fe;
					if(!$prj_info_arr[$avn]['sb'] || ($prj_info_arr[$avn]['sb'] > $sb))
						$prj_info_arr[$avn]['sb']=$sb;
					if(!$prj_info_arr[$avn]['se'] || ($prj_info_arr[$avn]['se'] < $se))
						$prj_info_arr[$avn]['se']=$se;

					$prj_info_arr[$avn]['gp']=$prj_info_arr[$avn]['sb']-$prj_info_arr[$avn]['fb'];
				}
				else if($sb > $prj_info_arr[$avn]['sb'])
					$sb=$prj_info_arr[$avn]['sb'];


				$pvn=$prj_info_arr[$avn]['pre'];
				if(!strlen($pvn))
					break;

				$avn=$pvn;
			}
		}
	}

	// Datum nachtragen, sowie Displayinformationen
	foreach($prj_info_arr as $vn => $data)
	{
		if($vn != 'var')
		{
			// Datum
			$prj_info_arr[$vn]['fbd']=time_add($prj_info_arr['var']['min_sdate'], $data['fb'], 'D');
			$prj_info_arr[$vn]['fed']=time_add($prj_info_arr['var']['min_sdate'], $data['fe'], 'D');
			$prj_info_arr[$vn]['sbd']=time_add($prj_info_arr['var']['min_sdate'], $data['sb'], 'D');
			$prj_info_arr[$vn]['sed']=time_add($prj_info_arr['var']['min_sdate'], $data['se'], 'D');

			// Displayinformationen
			// ...style
			if(!$data['type'])
				$prj_info_arr[$vn]['disp']['border-style']	= "dotted";
			// ...color
			if($with_cp && $data['cp'])
				$prj_info_arr[$vn]['disp']['border-color']	= "e1001a";
			// ...width
			if($with_cp && $data['cp'])
				$prj_info_arr[$vn]['disp']['border-width']	= 1;
			// ...image
			if(!$data['type'])
				$prj_info_arr[$vn]['disp']['image']			= "npp.png";
		}
	}
//fxDebug($prj_info_arr, '$prj_info_arr');

	return $prj_info_arr;
}

/**
 * ???FUNCTION???
 *
 * @param various $top_pid   - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $criterias - Optional parameter (default = false):  ???PARAMETER???
 * @param integer $dmode     - Optional parameter (default = 0):      ???PARAMETER???
 *
 * @return ???RETURN???
 */
function psp($top_pid, $criterias=false, $dmode=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Projekt-ID der obersten Ebene
	$top_pid=(int)$top_pid;
	if($top_pid <= 0)
		return false;

	// Puffertage und Krit. Pfad
	$buffer=0;
	$with_cp=false;
	if(is_array($criterias) && sizeof($criterias))
	{
		if(isset($criterias['v_buffer']) && $criterias['v_buffer'])
			$buffer=(int)$criterias['v_buffer'];
		if(isset($criterias['v_cpath']) && $criterias['v_cpath'])
			$with_cp=true;
	}

	// Mandanten-ID und Vorgangsnummer des übergebenen Projektes ermitteln
	$mid=$GLOBALS['fxpglobals']['client'];
	$top_vn='';
	$sql="SELECT vorgangsnummer FROM projekte WHERE projekt_id=".$top_pid." AND mandanten_id=".$mid;
	$tmp=db_values($sql);
//fxDebug($tmp, $sql);
	if(is_array($tmp))
		$top_vn=$tmp['vorgangsnummer'];
	if(!strlen($top_vn))
		return false;

	// Projektstrukturinfos ermitteln
	$sql  = "SELECT p.projekt_id, p.mutterprojekt_id, p.elter, p.vorgangsnummer, p.name_projekt, p.projektart, p.projekt_status, p.prioritaet, p.abhaengigkeit,";
	$sql .= " p.aufwand_soll, p.aufwand_ist, p.deadline, p.kunde, p.projektmanager, p.projektleiter, p.vertreter,";
	$sql .= " z.soll_beginn, z.soll_ende, z.ist_beginn, z.ist_ende,";
	$sql .= " b.budget_pers_int_gen, b.budget_pers_int_verpl, b.budget_pers_int_verbr, b.budget_mat_int_gen, b.budget_mat_int_verpl, b.budget_mat_int_verbr";
	$sql .= " FROM projekte p, zeitdaten z, budget_summe b";
	$sql .= " WHERE p.vorgangsnummer LIKE '".$top_vn."%' AND p.mandanten_id=".$mid." AND p.projektart<=".FXP_PRJ_TASK." AND z.projekt_id=p.projekt_id AND z.zeitart=102 AND z.mandanten_id=".$mid;
	$sql .= " AND b.projekt_id=p.projekt_id AND b.mandanten_id=".$mid;
	// ...Einschränkungskriterien
	if(is_array($criterias) && sizeof($criterias))
	{
		$tsql='';
		// ...Status
		if(isset($criterias['pst']) && strlen($criterias['pst']) && (!$criterias['pst'.FXP_PS_PLANNED] || !$criterias['pst'.FXP_PS_ACTIVE] || !$criterias['pst'.FXP_PS_INACTIVE] || !$criterias['pst'.FXP_PS_COMPLETED]))
			$tsql .= " AND p.projekt_status IN (".$criterias['pst'].")";
		// ...Hauptverantwortliche
		if(isset($criterias['pl']) && ($criterias['pl'] > 0))
			$tsql .= " AND (p.projektleiter=".$criterias['pl']." OR p.vertreter=".$criterias['pl'].")";

		$bsql='';
		// ...Hauptverantwortliche
		if(isset($criterias['cust']) && ($criterias['cust'] > 0))
			$bsql .= " AND p.kunde=".$criterias['cust'];
		if(isset($criterias['pm']) && ($criterias['pm'] > 0))
			$bsql .= " AND (p.projektmanager IS NULL OR p.projektmanager=0 OR p.projektmanager=".$criterias['pm'].")";
		// ...Zeitraum
		if(isset($criterias['sd']) && strlen($criterias['sd']) && isset($criterias['ed']) && strlen($criterias['ed']))
			$bsql .= " AND (z.soll_beginn<='".$criterias['ed']."240000' AND z.soll_ende>='".$criterias['sd']."000000')";
		else if(isset($criterias['sd']) && strlen($criterias['sd']))
			$bsql .= " AND z.soll_ende>='".$criterias['sd']."000000'";
		else if(isset($criterias['ed']) && strlen($criterias['ed']))
			$bsql .= " AND z.soll_beginn<='".$criterias['ed']."240000'";

		if(strlen($tsql) || strlen($bsql))
			$sql .= " AND ((p.projektart<".FXP_PRJ_TASK.$bsql.") OR (p.projektart=".FXP_PRJ_TASK.$tsql.$bsql."))";
	}
	$sql .= " ORDER BY p.vorgangsnummer";
	$prj =db_values($sql, '*');
//fxDebug($prj, $sql);
	if(!is_array($prj))
		return false;

	// Variablen
	$vn_arr			= array();	// Array zum Speichern der Vorgangsnummern
	$prj_info_arr	= array();	// Array zum Speichern der Projektstrukturinfos
	$bdg			= strtoupper(substr($GLOBALS['fxpglobals']['settings']['budgetart'],0,1));

	$prj_info_arr=array(
		'var' => array(
			'dmode'		=> $dmode,			// Anzeigeart
			'columns'	=> 0,				// Anzahl der Spalten
			'lines'		=> 0,				// Anzahl der Linien
			'gaps'		=> 0,				// Anzahl der Abstände
			'cp_vn'		=> '',				// Finale Vorgangsnummer für "Kritischen Pfad"
			'ftl'		=> 0,				// Erste Zeilennummer der Aufgabenebene
			'lpc'		=> array(0 => 0),	// Zeilennummer der untersten Aufgabe pro Spalte
			'gpc'		=> array(0 => 0)	// Zeilennummer des untersten Teilprojekts pro Spalte
		)
	);

	// Vorgangsnummern-Array erstellen
	$ch_arr=array();
	foreach($prj as $idx => $dat)
	{
		$vn=$dat['vorgangsnummer'];
		$pvn='';
		if(strlen($vn) > 4)
			$pvn=substr($vn,0,-5);

		$ch_arr[$vn]=array('tasks'=>0, 'projects'=>0);
		if(strlen($pvn))
		{
			if($dat['projektart'] < FXP_PRJ_TASK)
				$ch_arr[$pvn]['projects']++;
			else
				$ch_arr[$pvn]['tasks']++;
		}

		$vn_arr[$vn]=$idx;
		if($dat['projektart'] < FXP_PRJ_TASK)
			$prj_info_arr['var']['ftl']=max($prj_info_arr['var']['ftl'],1+floor(strlen($vn)/5));
	}
//fxDebug($vn_arr, '$vn_arr');
//fxDebug($ch_arr, '$ch_arr');

	// Projektstrukturplan
	foreach($vn_arr as $vn => $pkey)
	{
		$pdata=$prj[$pkey];

		$ptype=1;
		if($pdata['projektart'] < FXP_PRJ_TASK)
			$ptype=0;

		$level=floor(strlen($vn)/5);

		$pparent='';
		$column=-1;
		$line=0;
		$gap=0;
		if(strlen($vn) > 4)
		{
			$pparent=substr($vn,0,-5);

			if(!$dmode)
			{
				$cmgap=2;
				if(strlen($pparent) == 4)
				{
					$column=max(0,$prj_info_arr['var']['columns']);
					$prj_info_arr['var']['columns']=$column+1;

					$prj_info_arr['var']['lpc'][$column]=0;
					if(!isset($prj_info_arr['var']['gpc'][$column]))
					{
						if($column)
							$prj_info_arr['var']['gpc'][$column]=$cmgap;
						else
							$prj_info_arr['var']['gpc'][$column]=0;
					}
				}
				else
				{
					$column=max(0,$prj_info_arr[$pparent]['column']);
					$gap=$prj_info_arr[$pparent]['gap']+1;
					if(!isset($prj_info_arr['var']['gpc'][$column+1]))
						$prj_info_arr['var']['gpc'][$column+1]=$cmgap;
					$prj_info_arr['var']['gpc'][$column+1]=max($prj_info_arr['var']['gpc'][$column+1], $cmgap+$gap);
				}

				$line=$prj_info_arr['var']['lpc'][$column]+1;
				$prj_info_arr['var']['lpc'][$column]++;
			}
			else
			{
				$cmgap=2;
				if(!$ptype)
				{
					$column=max(0,$prj_info_arr['var']['columns']);
					if($ch_arr[$vn]['tasks'])
					{
						$prj_info_arr['var']['columns']++;
						if(!$column && $ch_arr[$pparent]['tasks'])
						{
							$column++;
							$prj_info_arr['var']['columns']++;
						}
					}

					$line=$level;
					if(!isset($prj_info_arr['var']['gpc'][$column]))
					{
						if($line == 1)
							$prj_info_arr['var']['gpc'][$column]=$cmgap;
						else
							$prj_info_arr['var']['gpc'][$column]=0;
					}

					$prj_info_arr['var']['lpc'][$column]=$prj_info_arr['var']['ftl'];
//echo('<b class="blue">'.$vn.'</b>: $line='.$line.', $column='.$column.', $gap='.$gap.' -- <i class="grey">var[columns]='.$prj_info_arr['var']['columns'].'</i><br />');
				}
				else
				{
					$column=max(0,$prj_info_arr[$pparent]['column']);
					$line=max($prj_info_arr['var']['ftl'],$prj_info_arr['var']['lpc'][$column]);

					$gap=1;

					$prj_info_arr['var']['lpc'][$column]=$line+1;
//echo('<b class="darkgrey">'.$vn.'</b>: $line='.$line.', $column='.$column.', $gap='.$gap.'<br />');
				}
			}

			$prj_info_arr['var']['lines']=max($prj_info_arr['var']['lines'], $line);

			$pvn=$pparent;
			while(strlen($pvn) > 0)
			{
				$prj_info_arr[$pvn]['lcolumn']=min($prj_info_arr[$pvn]['lcolumn'],$column);
				$prj_info_arr[$pvn]['rcolumn']=max($prj_info_arr[$pvn]['rcolumn'],$column);
				$pvn=substr($pvn,0,-5);
			}
		}

		$prj_info_arr[$vn]=array
		(
			'nr'		=> no_null($vn),
			'pid'		=> $pdata['projekt_id'],
			'mpid'		=> $pdata['mutterprojekt_id'],
			'name'		=> $pdata['name_projekt'],
			'type'		=> $ptype,
			'ptype'		=> (int)$pdata['projektart'],
			'stat'		=> (int)$pdata['projekt_status'],
			'prio'		=> (int)$pdata['prioritaet'],
			'effp'		=> $pdata['aufwand_soll']/3600,
			'effa'		=> $pdata['aufwand_ist']/3600,
			'bapp'		=> $pdata['budget_pers_int_gen']+$pdata['budget_mat_int_gen'],
			'buse'		=> $pdata['budget_pers_int_verpl']+$pdata['budget_pers_int_verbr']+$pdata['budget_mat_int_verpl']+$pdata['budget_mat_int_verbr'],

			'parent'	=> $pparent,
			'line'		=> $line,
			'column'	=> $column,
			'gap'		=> $gap,
			'cp'		=> false,
			'lcolumn'	=> max(0,$column),
			'rcolumn'	=> max(0,$column),
			'dprojects'	=> $ch_arr[$vn]['projects'],
			'dtasks'	=> $ch_arr[$vn]['tasks'],

			'ps'		=> substr($pdata['soll_beginn'], 0, 8),
			'pe'		=> substr($pdata['soll_ende'], 0, 8),
			'as'		=> substr($pdata['ist_beginn'], 0, 8),
			'ae'		=> substr($pdata['ist_ende'], 0, 8),

			'disp'		=> array('border-style'=>"solid", 'border-color'=>"aaaaaa", 'image'=>"", 'image-add'=>""),
			'tt'		=> ""
		);

		// Prüfen, ob Internes Budget angezeigt werden darf
		$kst=darf_kosten_sehen($pdata['projekt_id']);
		$hide_budget=true;
		if(($kst&1) && (($bdg == 'I') || ($bdg == 'B')))	// Internes Budget
			$hide_budget=false;
		if($hide_budget)
		{
			unset($prj_info_arr[$vn]['bapp']);
			unset($prj_info_arr[$vn]['buse']);
		}
	}

	foreach($prj_info_arr['var']['gpc'] as $column => $gap)
		$prj_info_arr['var']['gaps'] += $gap;
	if($dmode)
		$prj_info_arr['var']['gaps']++;

	// Kritischer Pfad
	if($with_cp)
	{
		$np=netplan($top_pid, false, false, $prj);
		if(is_array($np) && sizeof($np) && isset($np['var']))
		{
			$vn=$np['var']['cp_vn'];
			$prj_info_arr['var']['cp_vn']=$vn;
			while(strlen($vn))
			{
				$prj_info_arr[$vn]['cp']=true;
				$vn=$prj_info_arr[$vn]['parent'];
			}
		}
	}

	// Elementrahmen, Elementhintergrund und Tooltip
	$today		= substr($GLOBALS['datetime'],0,8);
	$today_mb	= substr(time_add($today, -$buffer, 'd'), 0, 8);
	$today_pb	= substr(time_add($today, $buffer, 'd'), 0, 8);

	$stat_array	= fxf_getReferenceValues('projekt_status');		// Projektstatusbezeichnungen
	$txt_sf		= explode('|', meldung(2834, false));	// Der Vorgang wurde BEGONNEN am: {as}.|Der Vorgang wurde noch NICHT begonnen.|Der Vorgang wurde ABGESCHLOSSEN am: {as}.|Der Vorgang wurde noch NICHT abgeschlossen.
	$txt_stat	= meldung(2835, false);					// Der Vorgang ist: {stat}
	$txt_start	= explode('|', get_message(189));		// [PSP_Start]	Das geplante Startdatum des Vorgangs (= {ps}) ist grösser als das heutige Datum{buffer} (= {tb})|Das geplante Startdatum des Vorgangs (= {ps}) ist kleiner oder gleich dem heutigen Datum{buffer} (= {tb})
	$txt_end	= explode('|', get_message(190));		// [PSP_End]	Das geplante Enddatum des Vorgangs (= {pe}) ist kleiner als das heutige Datum{buffer} (= {tb})|Das geplante Enddatum des Vorgangs (= {pe}) liegt in der Zukunft, aber es ist kleiner als das heutige Datum{buffer} (= {tb})|Das geplante Enddatum des Vorgangs (= {pe}) ist grösser oder gleich dem heutigen Datum{buffer} (= {tb})
	$txt_note	= explode('|', get_message(191));		// [PSP_Note]	<b>OK!</b> Zukünftiger Vorgang liegt gut im Plan|<b>OK!</b> Zukünftiger Vorgang liegt im Plan|<b>OK!</b> Vorgang liegt im Plan.|<b>HINWEIS!</b> Vorgang liegt im Plan, aber er wurde noch nicht begonnen.|<b>HINWEIS!</b> Vorgang sollte überprüft werden.|<b>HINWEIS!</b> Vorgang sollte angepasst werden!|<b>ACHTUNG!</b> Vorgang ist kritisch und muss angepasst werden!|<b>WARNUNG!</b> Vorgang ist extrem kritisch und muss angepasst werden!
	$txt_bud	= explode('|', get_message(193));		// [PSP_Budget]	Für dieses Projekt wurde (noch) kein int. Budget verbraucht.|Für dieses Projekt wurde (noch) kein int. Budget genehmigt!|Das verbrauchte int. Gesamtbudget (= {bv}) ist kleiner als das genehmigte int. Gesamtbudget (= {bg}).|Das verbrauchte int. Gesamtbudget (= {bv}) ist bereits größer als {bp}% des genehmigten int. Gesamtbudgets (= {bgp})!|Das verbrauchte int. Gesamtbudget (= {bv}) ist größer als das genehmigte int. Gesamtbudget (= {bg})!
	$txt_bnote	= explode('|', get_message(194));		// [PSP_BNote]	<b>OK!</b> Budgetierung liegt im Plan.|<b>HINWEIS!</b> Budgetierung sollte im Auge behalten werden!|<b>HINWEIS!</b> Budgetierung sollte durchgeführt werden!|<b>ACHTUNG!</b> Budgetierung muss neu durchkalkuliert werden!
	$txt_eff	= explode('|', get_message(195));		// [PSP_Effort]	Für diesen Vorgang wurde (noch) kein Aufwand verbraucht.|Für diesen Vorgang wurde (noch) kein Aufwand geplant!|Der verbrauchte Aufwand (= {eac}) ist kleiner als der geplante Aufwand (= {epl}).|Der verbrauchte Aufwand (= {eac}) ist bereits größer als {eps}% des geplanten Aufwands (= {epp})!|Der verbrauchte Aufwand (= {eac}) ist größer als der geplante Aufwand!
	$txt_enote	= explode('|', get_message(196));		// [PSP_ENote]	<b>OK!</b> Aufwandsverbrauch liegt im Plan.|<b>HINWEIS!</b> Aufwandsverbrauch sollte im Auge behalten werden!|<b>HINWEIS!</b> Aufwandsplanung sollte durchgeführt werden!|<b>ACHTUNG!</b> Aufwandsplanung muss neu durchkalkuliert werden!

	$txt_mb		= '';
	$txt_pb		= '';
	if($buffer)
	{
		$txt_pt	= explode('|', meldung(2832, false, $buffer));	// abzüglich $var1 Puffertage(e)|zuzüglich $var1 Puffertag(e)
		$txt_mb=' '.$txt_pt[0];
		$txt_pb=' '.$txt_pt[1];
	}
	foreach($prj_info_arr as $vn => $data)
	{
		if($vn != 'var')
		{
			// Border
			if(strlen($prj_info_arr['var']['cp_vn']) && $prj_info_arr[$vn]['cp'])
			{
				if(($data['stat'] == FXP_PS_PLANNED) || ($data['stat'] == FXP_PS_INACTIVE))
				{
					$prj_info_arr[$vn]['disp']['border-style']="dotted";
					$prj_info_arr[$vn]['disp']['border-color']="e1001a";
				}
				else
					$prj_info_arr[$vn]['disp']['border-color']="cc4444";
			}
			else if(($data['stat'] == FXP_PS_PLANNED) || ($data['stat'] == FXP_PS_INACTIVE))
			{
				$prj_info_arr[$vn]['disp']['border-style']="dotted";
				$prj_info_arr[$vn]['disp']['border-color']="888888";
			}
			else if($data['stat'] == FXP_PS_COMPLETED)
				$prj_info_arr[$vn]['disp']['border-color']="000000";

			// Vorgang begonnen?
			if($prj_info_arr[$vn]['as'])
			{
				// Der Vorgang wurde BEGONNEN am: {as}.
				$ptt='&nbsp;'.str_replace('{as}', '<b>'.get_entry($data['as'], "datum").'</b>', $txt_sf[0]).'&nbsp;';
			}
			else
			{
				// Der Vorgang wurde noch NICHT begonnen.
				$ptt='&nbsp;'.$txt_sf[1].'&nbsp;';
			}
			$ptt .= '<hr class=fxhr>';

			// Vorgang INAKTIV
			if($data['stat'] == FXP_PS_INACTIVE)
			{
				// Der Vorgang ist: {stat}
				$ptt .= '&nbsp;'.str_replace('{stat}', '<b>'.strtoupper($stat_array[$data['stat']]).'</b>', $txt_stat).'&nbsp;';
				$prj_info_arr[$vn]['disp']['image']="in";
				// Vorgang gestartet, dann Linie von links unten nach rechts oben
				if($data['as'])
					$prj_info_arr[$vn]['disp']['image-add']="_s";
			}
			// Vorgang ABGESCHLOSSEN
			else if($data['stat'] == FXP_PS_COMPLETED)
			{
				if(strlen($data['ae']))
				{
					// Der Vorgang wurde ABGESCHLOSSEN am: {ae}.
					$ptt .= '&nbsp;'.str_replace('{ae}', '<b>'.get_entry($data['ae'], "datum").'</b>', $txt_sf[2]).'&nbsp;';
				}
				else
				{
					// Der Vorgang ist: {stat}
					$ptt .= '&nbsp;'.str_replace('{stat}', '<b>'.strtoupper($stat_array[$data['stat']]).'</b>', $txt_stat).'&nbsp;';
				}
				// Gekreuzte Linien (grau)
				$prj_info_arr[$vn]['disp']['image']="in";
				$prj_info_arr[$vn]['disp']['image-add']="_f";
			}
			// Vorgang GEPLANT oder AKTIV
			else
			{
				// ...Zeitraum
				$std=array('{ps}'=>get_entry($data['ps'], "datum"), '{pe}'=>get_entry($data['pe'], "datum"), '{buffer}'=>"", '{tb}'=>get_entry($today, "datum"));
				$smb=array('{ps}'=>get_entry($data['ps'], "datum"), '{pe}'=>get_entry($data['pe'], "datum"), '{buffer}'=>$txt_mb, '{tb}'=>get_entry($today_mb, "datum"));
				$spb=array('{ps}'=>get_entry($data['ps'], "datum"), '{pe}'=>get_entry($data['pe'], "datum"), '{buffer}'=>$txt_pb, '{tb}'=>get_entry($today_pb, "datum"));

				if($data['ps'] > $today_pb)
				{
					// Das geplante Startdatum des Vorgangs (= {ps}) ist grösser als das heutige Datum{buffer} (= {tb})
					$ptt .= '&nbsp;'.strtr($txt_start[0], $spb).'&nbsp;<br />';
					// <b>OK!</b> Zukünftiger Vorgang liegt gut im Plan
					$ptt .= '&nbsp;-&gt; '.$txt_note[0].'&nbsp;';
				}
				else if($data['ps'] > $today)
				{
					// Das geplante Startdatum des Vorgangs (= {ps}) ist grösser als das heutige Datum{buffer} (= {tb})
					$ptt .= '&nbsp;'.strtr($txt_start[0], $std).'&nbsp;<br />';
					// <b>OK!</b> Zukünftiger Vorgang liegt im Plan
					$ptt .= '&nbsp;-&gt; '.$txt_note[1].'&nbsp;';
				}
				else
				{
					// Das geplante Startdatum des Vorgangs (= {ps}) ist kleiner oder gleich dem heutigen Datum{buffer} (= {tb})
					$ptt .= '&nbsp;'.strtr($txt_start[1], $std).'&nbsp;<br />';
					if($data['pe'] < $today_mb)
					{
						// Das geplante Enddatum des Vorgangs (= {pe}) ist kleiner als das heutige Datum{buffer} (= {tb})
						$ptt .= '&nbsp;'.strtr($txt_end[0], $smb).'&nbsp;<br />';
						// <b>WARNUNG!</b> Vorgang ist extrem kritisch und muss angepasst werden!
						$ptt .= '&nbsp;-&gt; '.$txt_note[7].'&nbsp;';
						$prj_info_arr[$vn]['disp']['image']="ko";
					}
					else if($data['pe'] < $today)
					{
						if($prj_info_arr[$vn]['as'])
						{
							// Das geplante Enddatum des Vorgangs (= {pe}) ist kleiner als das heutige Datum{buffer} (= {tb})
							$ptt .= '&nbsp;'.strtr($txt_end[0], $std).'&nbsp;<br />';
							// <b>HINWEIS!</b> Vorgang sollte angepasst werden!
							$ptt .= '&nbsp;-&gt; '.$txt_note[5].'&nbsp;';
							$prj_info_arr[$vn]['disp']['image']="wn";
						}
						else
						{
							// Das geplante Enddatum des Vorgangs (= {pe}) ist kleiner als das heutige Datum{buffer} (= {tb})
							$ptt .= '&nbsp;'.strtr($txt_end[0], $std).'&nbsp;<br />';
							// <b>ACHTUNG!</b> Vorgang ist kritisch und muss angepasst werden!
							$ptt .= '&nbsp;-&gt; '.$txt_note[6].'&nbsp;';
							$prj_info_arr[$vn]['disp']['image']="ko";
						}
					}
					else if($data['pe'] < $today_pb)
					{
						// Das geplante Enddatum des Vorgangs (= {pe}) liegt in der Zukunft, aber es ist kleiner als das heutige Datum{buffer} (= {tb})
						$ptt .= '&nbsp;'.strtr($txt_end[1], $spb).'&nbsp;<br />';
						// <b>HINWEIS!</b> Vorgang sollte überprüft werden.
						$ptt .= '&nbsp;-&gt; '.$txt_note[4].'&nbsp;';
						$prj_info_arr[$vn]['disp']['image']="wn";
					}
					else
					{
						if($prj_info_arr[$vn]['as'])
						{
							// Das geplante Enddatum des Vorgangs (= {pe}) ist grösser oder gleich dem heutigen Datum{buffer} (= {tb})
							$ptt .= '&nbsp;'.strtr($txt_end[2], $spb).'&nbsp;<br />';
							// <b>OK!</b> Vorgang liegt im Plan.
							$ptt .= '&nbsp;-&gt; '.$txt_note[2].'&nbsp;';
							$prj_info_arr[$vn]['disp']['image']="ok";
						}
						else
						{
							// Das geplante Enddatum des Vorgangs (= {pe}) ist grösser oder gleich dem heutigen Datum{buffer} (= {tb})
							$ptt .= '&nbsp;'.strtr($txt_end[2], $spb).'&nbsp;<br />';
							// <b>HINWEIS!</b> Vorgang liegt im Plan, aber er wurde noch nicht begonnen.
							$ptt .= '&nbsp;-&gt; '.$txt_note[3].'&nbsp;';
							$prj_info_arr[$vn]['disp']['image']="wn";
						}
					}
				}

				// Budget
				if(isset($data['bapp']) && isset($data['buse']) && ($data['ps'] <= $today))
				{
					$bpm_ps		= 80.0;
					$bpm_gen	= $data['bapp'];
					$bpm_genp	= ($bpm_gen*$bpm_ps)/100.0;
					$bpm_ver	= $data['buse'];

					$bud=array('{bp}'=>get_entry($bpm_ps, "dezimal"), '{bg}'=>get_entry($bpm_gen, "dezimal").' '.$GLOBALS['fxpglobals']['settings']['waehrung'], '{bgp}'=>get_entry($bpm_genp, "dezimal").' '.$GLOBALS['fxpglobals']['settings']['waehrung'], '{bv}'=>get_entry($bpm_ver, "dezimal").' '.$GLOBALS['fxpglobals']['settings']['waehrung']);

					$ptt .= '<hr class=fxhr>';
					if(!$bpm_gen)
					{
						// 1: Für diesen Vorgang wurde (noch) kein int. Budget genehmigt!
						$ptt .= '&nbsp;'.$txt_bud[1].'&nbsp;<br />';
						// 2: <b>HINWEIS!</b> Budgetierung sollte durchgeführt werden!
						$ptt .= '&nbsp;-&gt; '.$txt_bnote[2].'&nbsp;';
						if($prj_info_arr[$vn]['disp']['image'] == "ok")
							$prj_info_arr[$vn]['disp']['image']="wn";
					}
					else if($bpm_ver > $bpm_gen)
					{
						// 4: Das verbrauchte int. Gesamtbudget (= {bv}) ist größer als das genehmigte int. Gesamtbudget (= {bg})!
						$ptt .= '&nbsp;'.strtr($txt_bud[4], $bud).'&nbsp;<br />';
						// 3: <b>ACHTUNG!</b> Budgetierung muss neu durchkalkuliert werden!
						$ptt .= '&nbsp;-&gt; '.$txt_bnote[3].'&nbsp;';
						if($prj_info_arr[$vn]['disp']['image'] != "ko")
							$prj_info_arr[$vn]['disp']['image']="ko";
					}
					else if($bpm_ver > $bpm_genp)
					{
						// 3: Das verbrauchte int. Gesamtbudget (= {bv}) ist bereits größer als {bp}% des genehmigten int. Gesamtbudgets (= {bgp})!
						$ptt .= '&nbsp;'.strtr($txt_bud[3], $bud).'&nbsp;<br />';
						// 1: <b>HINWEIS!</b> Budgetierung sollte im Auge behalten werden!
						$ptt .= '&nbsp;-&gt; '.$txt_bnote[1].'&nbsp;';
						if($prj_info_arr[$vn]['disp']['image'] == "ok")
							$prj_info_arr[$vn]['disp']['image']="wn";
					}
					else if($bpm_ver)
					{
						// 2: Das verbrauchte int. Gesamtbudget (= {bv}) ist kleiner als das genehmigte int. Gesamtbudget (= {bg}).
						$ptt .= '&nbsp;'.strtr($txt_bud[2], $bud).'&nbsp;<br />';
						// 0: <b>OK!</b> Budgetierung liegt im Plan.
						$ptt .= '&nbsp;-&gt; '.$txt_bnote[0].'&nbsp;';
					}
					else
					{
						// 0: Für diesen Vorgang wurde (noch) kein int. Budget verbraucht!
						$ptt .= '&nbsp;'.$txt_bud[0].'&nbsp;';
					}
				}

				// Aufwand
				if($data['ps'] <= $today)
				{
					$eff_ps	= 80.0;
					$eff_pl	= $data['effp'];
					$eff_pp	= ($eff_pl*$eff_ps)/100.0;
					$eff_ac	= $data['effa'];

					$eff=array('{eps}'=>get_entry($eff_ps, "dezimal"), '{epl}'=>get_entry($eff_pl, "dezimal").' h', '{epp}'=>get_entry($eff_pp, "dezimal").' h', '{eac}'=>get_entry($eff_ac, "dezimal").' h');

					$ptt .= '<hr class=fxhr>';
					if(!$eff_pl)
					{
						// 1: Für diesen Vorgang wurde (noch) kein Aufwand geplant!
						$ptt .= '&nbsp;'.$txt_eff[1].'&nbsp;<br />';
						// 2: <b>HINWEIS!</b> Aufwandsplanung sollte durchgeführt werden!
						$ptt .= '&nbsp;-&gt; '.$txt_enote[2].'&nbsp;';
						if($prj_info_arr[$vn]['disp']['image'] == "ok")
							$prj_info_arr[$vn]['disp']['image']="wn";
					}
					else if($eff_ac > $eff_pl)
					{
						// 4: Der verbrauchte Aufwand (= {eac}) ist größer als der geplante Aufwand (= {epl})!
						$ptt .= '&nbsp;'.strtr($txt_eff[4], $eff).'&nbsp;<br />';
						// 3: <b>ACHTUNG!</b> Aufwandsplanung muss neu durchkalkuliert werden!
						$ptt .= '&nbsp;-&gt; '.$txt_enote[3].'&nbsp;';
						if($prj_info_arr[$vn]['disp']['image'] != "ko")
							$prj_info_arr[$vn]['disp']['image']="ko";
					}
					else if($eff_ac > $eff_pp)
					{
						// 3: Der verbrauchte Aufwand (= {eac}) ist bereits größer als {eps}% des geplanten Aufwands (= {epp})!
						$ptt .= '&nbsp;'.strtr($txt_eff[3], $eff).'&nbsp;<br />';
						// 1: <b>HINWEIS!</b> Aufwandsverbrauch sollte im Auge behalten werden!
						$ptt .= '&nbsp;-&gt; '.$txt_enote[1].'&nbsp;';
						if($prj_info_arr[$vn]['disp']['image'] == "ok")
							$prj_info_arr[$vn]['disp']['image']="wn";
					}
					else if($eff_ac)
					{
						// 2: Der verbrauchte Aufwand (= {eac}) ist kleiner als der geplante Aufwand (= {epl})!
						$ptt .= '&nbsp;'.strtr($txt_eff[2], $eff).'&nbsp;<br />';
						// 0: <b>OK!</b> Aufwandsverbrauch liegt im Plan.
						$ptt .= '&nbsp;-&gt; '.$txt_enote[0].'&nbsp;';
					}
					else
					{
						// 0: Für diesen Vorgang wurde (noch) kein Aufwand verbraucht!
						$ptt .= '&nbsp;'.$txt_eff[0].'&nbsp;';
					}
				}

				// Vorgang begonnen, dann Linie links unten nach rechts oben
				if($prj_info_arr[$vn]['as'])
					$prj_info_arr[$vn]['disp']['image-add']="_s";
			}

			$prj_info_arr[$vn]['tt']=$ptt;
		}
	}

//fxDebug($prj_info_arr, '$prj_info_arr');
	return $prj_info_arr;
}

/**
 * Funktion ermittelt die möglichen Minimum- und Maximumtermine eines Projektes
 *
 * @param various $pid - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getMinMaxDates($pid)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$this_debug=false;
if($this_debug) echo('<div style="background:#ffffff;border:1px solid black;border-top-left-radius:6px;border-top-right-radius:6px;padding:3px;margin:1px;"><b class="blue">function</b> getMinMaxDates($pid='.$pid.', $with_parents='.$with_parents.')</div>');

	$mda=array('minstart'=>"00000000", 'maxstart'=>"99999999", 'minend'=>"00000000", 'maxend'=>"99999999");

	$pid=(int)$pid;
	// Nur auf Abhängigkeiten prüfen, falls Projekt-ID übermittelt wurde
	if($pid > 0)
	{
		$vn=db_value("SELECT vorgangsnummer FROM projekte WHERE projekt_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);

		$vc="vorgangsnummer LIKE '".$vn."'";
		while(strlen($vn) > 5)
		{
			$vn=substr($vn,0,-5);
			$vc .= " OR vorgangsnummer LIKE '".$vn."'";
		}
		$vc="SELECT projekt_id FROM projekte WHERE (".$vc.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$va=db_values($vc,'*');
if($this_debug) fxDebug($va,$vc);

		$sql  = "SELECT a.projekt_a, za.soll_beginn AS start_a, za.soll_ende AS ende_a, a.projekt_b, zb.soll_beginn AS start_b, zb.soll_ende AS ende_b, a.abh_typ, a.tagesanzahl";
		$sql .= " FROM abhaengigkeiten a, zeitdaten za, zeitdaten zb";
		$sql .= " WHERE (a.projekt_a IN (".$vc.") OR a.projekt_b IN (".$vc.")) AND a.mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$sql .= " AND za.projekt_id=a.projekt_a AND za.zeitart=102 AND za.mandanten_id=a.mandanten_id AND zb.projekt_id=a.projekt_b AND zb.zeitart=102 AND zb.mandanten_id=a.mandanten_id";
		$aa=db_values($sql,'*');
if($this_debug) fxDebug($aa,$sql);

		if(fxIsArray($aa))
		{
			foreach($va as $v)
			{
				$id=$v['projekt_id'];
				foreach($aa as $a)
				{
					switch($a['abh_typ'])
					{
						case 'SS':	// (SS) Start-to-Start
							if($a['projekt_a'] == $id)
							{
								$mda['maxstart']=min($mda['maxstart'], substr($a['start_b'],0,8));
								$mda['maxend']=min($mda['maxend'], $mda['maxstart']);
							}
							else if($a['projekt_b'] == $id)
							{
								$mda['minstart']=max($mda['minstart'], substr($a['start_a'],0,8));
								$mda['minend']=max($mda['minend'], $mda['minstart']);
							}
						break;

						case 'SF':	// (SF) Start-to-Finish
							if($a['projekt_a'] == $id)
							{
								$mda['maxstart']=min($mda['maxstart'], substr(time_add($a['ende_b'],1,'d'),0,8));
								$mda['maxend']=min($mda['maxend'], $mda['maxstart']);
							}
							else if($a['projekt_b'] == $id)
							{
								$mda['minend']=max($mda['minende'], substr(time_add($a['start_a'],-1,'d'),0,8));
								$mda['minstart']=max($mda['minstart'], $mda['minende']);
							}
						break;

						case 'FF':	// (FF) Finish-to-Finish
							if($a['projekt_a'] == $id)
							{
								$mda['maxend']=min($mda['maxend'], substr($a['ende_b'],0,8));
								$mda['maxstart']=min($mda['maxstart'], $mda['maxend']);
							}
							else if($a['projekt_b'] == $id)
							{
								$mda['minend']=max($mda['minend'], substr($a['ende_a'],0,8));
								$mda['minstart']=max($mda['minstart'], $mda['minend']);
							}
						break;

						case 'FS':	// (FS) Finish-to-Start
							if($a['projekt_a'] == $id)
							{
								$mda['maxend']=min($mda['maxend'], substr(time_add($a['start_b'],-1,'d'),0,8));
								$mda['maxstart']=min($mda['maxstart'], $mda['maxend']);
							}
							else if($a['projekt_b'] == $id)
							{
								$mda['minstart']=max($mda['minstart'], substr(time_add($a['ende_a'],1,'d'),0,8));
								$mda['minend']=max($mda['minend'], $mda['minstart']);
							}
						break;
					}
				}
			}
		}
	}

if($this_debug) fxDebug($mda,'$mda');
	return $mda;
}

/**
 * Funktion passt das Datum der zugeordneten Meilensteine eines jeden Projektes auf das entspr. Start- oder Enddatum des Projekts an
 *
 * @param various $pid - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function setMilestoneDates($pid)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$this_debug=false;
if($this_debug) echo('<div style="background:#ffffff;border:1px solid black;border-top-left-radius:6px;border-top-right-radius:6px;padding:3px;margin:1px;"><b class="blue">function</b> setMilestoneDates($pid='.$pid.')</div>');

	$pid=(int)$pid;
	// Wenn keine Projekt-ID übergeben wurde, dann Funktion mit Fehlerkennzeichen verlassen
	if(!$pid)
		return true;

	// Mutterprojekt ermitteln
	$sql="SELECT mutterprojekt_id FROM projekte WHERE projekt_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$mpid=(int)db_value($sql);
if($this_debug) fxDebug($mpid,$sql);
	// Wenn das Projekt kein Mutterprojekt hat, dann Funktion mit Fehlerkennzeichen verlassen
	if(!$mpid)
		return true;

	// SQL für "Alle untergeordneten Projekte inkl. des aktuellen Projektes"
	$inf="SELECT projekt_id FROM projekte WHERE mutterprojekt_id=".$mpid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];

	// Alle dynamischen Meilensteintermine ermitteln
	$sql="SELECT meilenstein_id, projekt_id, soll_ende, statusfeld, tagesanzahl FROM meilensteine WHERE projekt_id IN (".$inf.") AND statusfeld<>0 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$tmp=db_values($sql,'*');
if($this_debug) fxDebug($tmp,$sql);
	// Wenn es keine Meilensteintermine gibt, dann Funktion mit Erfolgskennzeichen verlassen
	if(!fxIsArray($tmp))
		return false;

	// Meilensteintermine in benutzbares Array-Format bringen
	$milestones=array();
	$mpc=array();
	foreach($tmp as $t)
	{
		$tpid=(int)$t['projekt_id'];
		if(!isset($milestones[$tpid]))
		{
			$milestones[$tpid]=$t;
			$mpc[$tpid]=1;
		}
		else
		{
			$milestones[$tpid.'_'.$mpc[$tpid]]=$t;
			$mpc[$tpid]++;
		}
	}
if($this_debug) fxDebug($milestones,'$milestones');

	// Alle Projekttermine ermitteln
	$sql="SELECT projekt_id, soll_beginn, soll_ende FROM zeitdaten WHERE projekt_id IN (".$inf.") AND zeitart=102 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$tmp=db_values($sql,'*');
if($this_debug) fxDebug($tmp,$sql);
	// Wenn es keine Projekttermine gibt, dann Funktion mit Erfolgskennzeichen verlassen
	if(!fxIsArray($tmp))
		return false;

	// Projekttermine in benutzbares Array-Format bringen
	$projects=array();
	foreach($tmp as $t)
		$projects[(int)$t['projekt_id']]=array('s'=>substr($t['soll_beginn'],0,8), 'e'=>substr($t['soll_ende'],0,8));
if($this_debug) fxDebug($projects,'$projects');

	// Meilensteintermine mit Projektterminen vergleichen und gegebenenfalls zum Ändern merken
	$change=array();
	foreach($milestones as $mpid => $mdata)
	{
		$us_pos=strpos($mpid, '_');
		if($us_pos !== false)
			$mpid=substr($mpid,0,$us_pos);
		$mpid=(int)$mpid;

		// Termine unterschiedlich? Ja, dann merken
		if(isset($projects[$mpid]))
		{
			$mnd='';
			$mcd=substr($mdata['soll_ende'],0,8);					// Bsp: 20130822
			if($mdata['tagesanzahl'] != 0)							// Bsp: +2
				$mcd=time_add($mcd,-$mdata['tagesanzahl'],'d');		// Bsp: 20130822 - (+2)=20130820

			if($mdata['statusfeld'] < 0)	// Meilenstein verbunden mit Startdatum
				$pcd=$projects[$mpid]['s'];
			else							// Meilenstein verbunden mit Enddatum
				$pcd=$projects[$mpid]['e'];
			if($pcd != $mcd)
			{
				$mnd=$pcd;
				if($mdata['tagesanzahl'] != 0)
					$mnd=time_add($mnd,$mdata['tagesanzahl'],'d');
			}
if($this_debug) echo('id='.$mpid.', typ='.$mdata['statusfeld'].', tage='.$mdata['tagesanzahl'].': $mcd='.$mcd.', $pcd='.$pcd.' - $mnd='.$mnd.'<br />');

			if(strlen($mnd))
				$change[$mdata['meilenstein_id']]="UPDATE meilensteine SET soll_ende='".substr($mnd,0,8)."000000' WHERE meilenstein_id=".$mdata['meilenstein_id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		}
	}
if($this_debug) fxDebug($change,'$change');

	// Gibt es Änderungen, dann die Meilensteintermine anpassen
	if(fxIsArray($change))
	{
		foreach($change as $mid => $sql)
		{
			$err=db_x($sql);
if($this_debug) fxDebug($err,$sql);

			// Trat beim Anpassen des Meilensteintermin ein Fehler auf, dann Funktion mit Fehlerkennzeichen verlassen
			if($err)
				return true;
		}
	}

	// Alle erfolgreich abgeschlossen, dann Funktion mit Erfolgskennzeichen verlassen
	return false;
}

/**
 * ???FUNCTION???
 *
 * @param various $top_pid   - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $criterias - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function eva($top_pid, $criterias=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Projekt-ID der obersten Ebene
	$top_pid=(int)$top_pid;
	if($top_pid <= 0)
		return false;

	// Mandanten-ID und Vorgangsnummer des übergebenen Projektes ermitteln
	$mid	= $GLOBALS['fxpglobals']['client'];
	$top_vn	= '';
	$sql="SELECT vorgangsnummer, name_projekt FROM projekte WHERE projekt_id=".$top_pid." AND mandanten_id=".$mid;
	$tmp=db_values($sql);
//fxDebug($tmp, $sql);
	if(fxIsArray($tmp))
	{
		$top_vn		= $tmp['vorgangsnummer'];
		$top_name	= $tmp['name_projekt'];
	}
	if(!strlen($top_vn))
		return false;

	// Projektstrukturinfos der untergeordneten Aufgaben ermitteln
	$sql  = "SELECT p.projekt_id, p.vorgangsnummer, p.name_projekt, p.projektart, p.projekt_status,";
	$sql .= " p.aufwand_soll, p.aufwand_ist, p.kunde, p.projektmanager, p.projektleiter, p.vertreter,";
	$sql .= " z.soll_beginn, z.soll_ende, z.ist_beginn, z.ist_ende,";
	$sql .= " b.budget_pers_int_gepl, b.budget_pers_int_gen, b.budget_mat_int_gepl, b.budget_mat_int_gen,";
	$sql .= " b.budget_pers_ext_gepl, b.budget_pers_ext_gen, b.budget_mat_ext_gepl, b.budget_mat_ext_gen";
	$sql .= " FROM projekte p, zeitdaten z, budget_summe b";
	$sql .= " WHERE p.vorgangsnummer LIKE '".$top_vn."%' AND p.mandanten_id=".$mid." AND p.projektart=".FXP_PRJ_TASK." AND z.projekt_id=p.projekt_id AND z.zeitart=102 AND z.mandanten_id=".$mid;
	$sql .= " AND b.projekt_id=p.projekt_id AND b.mandanten_id=".$mid;
	// ...Einschränkungskriterien
	if(fxIsArray($criterias))
	{
		// ...Status
		if(isset($criterias['pst']) && ($criterias['pst'.FXP_PS_PLANNED] || $criterias['pst'.FXP_PS_ACTIVE] || $criterias['pst'.FXP_PS_INACTIVE] || $criterias['pst'.FXP_PS_COMPLETED]))
			$sql .= " AND p.projekt_status IN (".$criterias['pst'].")";
		// ...Hauptverantwortliche
		if(isset($criterias['pl']) && ($criterias['pl'] > 0))
			$sql .= " AND (p.projektleiter=".$criterias['pl']." OR p.vertreter=".$criterias['pl'].")";
		if(isset($criterias['cust']) && ($criterias['cust'] > 0))
			$sql .= " AND p.kunde=".$criterias['cust'];
		if(isset($criterias['pm']) && ($criterias['pm'] > 0))
			$sql .= " AND (p.projektmanager IS NULL OR p.projektmanager=0 OR p.projektmanager=".$criterias['pm'].")";
	}
	$sql .= " ORDER BY z.soll_beginn";
	$prj =db_values($sql, '*');
//fxDebug($prj, $sql);
	if(!fxIsArray($prj))
		return false;

	// Minimum- und Maximumzeitraum ermitteln
	$eva_arr=array(
		'inf'	=> array('nr'=>$top_vn, 'name'=>$top_name),

		'pe'	=> array(),	// Planned Effort (Sollaufwand)
		'ae'	=> array(),	// Actual Effort (Istaufwand)

		'pa'	=> array(),	// Percent Complete

		'pc'	=> array(),	// Planned Cost (Sollkosten)
		'ac'	=> array(),	// Actual Cost (Istkosten)
		'ev'	=> array(),	// Earned Value

		'sv'	=> array(),	// Scheduled Variance (Planabweichung)
		'cv'	=> array(),	// Cost Variance (Kostenabweichung)
		'spi'	=> array(),	// Schedule Performance Index (Zeiteffizienz)
		'cpi'	=> array()	// Cost Performance Index (Kosteneffizienz)
	);
	$time_arr	= array();
	$today		= substr($GLOBALS['datetime'],0,8);
	$min_date	= '99999999';
	$max_date	= '00000000';
	foreach($prj as $p)
	{
		$pid=(int)$p['projekt_id'];

		// Planzeitraum
		$psd=substr($p['soll_beginn'], 0, 8);
		$ped=substr($p['soll_ende'], 0, 8);

		// Istzeitraum
		$asd=substr($p['ist_beginn'], 0, 8);
		$aed=substr($p['ist_ende'], 0, 8);

		// Planbudget
		$pbud=get_double($p['budget_pers_int_gen']) + get_double($p['budget_mat_int_gen']);

		// Fortschrittsmethode
		$fpid=$pid;
		$fpid=0;	// Ermittlung der tatsächlichen Fortschrittsmethode pro Aufgabe momentan deaktiviert
		while($fpid)
		{
			$sql="SELECT methode FROM projektfortschritt WHERE projekt_id=".$fpid." AND (personen_id IS NULL OR personen_id=0) AND aktiv_kz=1 AND mandanten_id=".$mid;
			$em=(int)db_value($sql);
			if($em)
				break;

			$sql="SELECT elter FROM projekte WHERE projekt_id=".$fpid." AND mandanten_id=".$mid;
			$fpid=(int)db_value($sql);
			if(!$fpid)
			{
				$em=1784;
				break;
			}
		}
		$em=1784;	// momentan fest auf 1784=Vergleich Soll-/Ist-Aufwand (Standard)

		// Min- und Maxdatum
		$min_date=min($min_date, $psd);
		$max_date=max($max_date, $ped);
		if(strlen($asd))
			$min_date=min($min_date, $asd);
		if(strlen($aed))
			$max_date=max($max_date, $aed);
		// Falls aktive Aufgabe, dann max. Enddatum evtl. auf Heute setzen
		if($p['projekt_status'] == FXP_PS_ACTIVE)
			$max_date=max($max_date, $today);
		// Zeitinfos
		$time_arr[$pid]=array(
			'psd'	=> $psd,								// Geplanter Starttermin
			'ped'	=> $ped,								// Geplanter Endtermin
			'pdd'	=> 1+fxp_date_diff($psd, $ped, 'D'),	// Geplante Dauer in Tagen
			'asd'	=> $asd,								// Tatsächlicher Starttermin
			'aed'	=> $aed,								// Tatsächlicher Endtermin
			'pbud'	=> $pbud,								// Geplante Kosten
			'peff'	=> get_double($p['aufwand_soll']),		// Geplanter Aufwand in Sekunden
			'em'	=> $em									// Fortschrittsmethode (1784=Vergleich Soll-/Ist-Aufwand (Standard), 1785=Restaufwand in Stunden, 1786=Methode 0/50/100, 1787=Methode 0/100, 1788=Restaufwand Prozentsatz, 1789=Fertigstellung Prozentsatz
		);
	}
//fxDebug($time_arr, '$time_arr');

	// Zeiteinteilung
	$days=1+fxp_date_diff($min_date, $max_date, 'D');
	for($dc=0; $dc<=$days; $dc++)
	{
		$ddate=min($max_date, max($min_date, time_add($min_date, (int)$dc, 'D')));
		$eva_arr['pc'][$ddate]=0.0;
	}
//echo('$min_date=<b>'.$min_date.'</b>, $max_date=<b>'.$max_date.'</b> -&gt; $days=<b>'.$days.'</b><br />');

	// Plankosten (= Planned Cost "PC")
	// Planwert (= Planned Value "PV")
	// Soll-Kosten der berechneten Arbeit "SKBA" (= Budgeted Cost of Work Scheduled "BCWS")
	foreach($eva_arr['pc'] as $pdate => $dummy)
	{
		$pe=0.0;
		$pc=0.0;

		foreach($time_arr as $tpid => $t)
		{
			// Vergleichsdatum >= Gepl. Startdatum
			if($pdate >= $t['psd'])
			{
				// Vergleichsdatum >= Gepl. Enddatum, dann ganzes Planbudget hinzuaddieren
				if($pdate >= $t['ped'])
				{
					$pe += $t['peff'];
					$pc += $t['pbud'];
				}
				else
				{
					$dd=1+fxp_date_diff($t['psd'], $pdate, 'D');

					$pe += round(($t['peff']/$t['pdd'])*$dd, 2);
					$pc += round(($t['pbud']/$t['pdd'])*$dd, 2);
				}
			}
		}

		$eva_arr['pe'][$pdate]	= $pe;
		$eva_arr['ae'][$pdate]	= 0.0;

		$eva_arr['pa'][$pdate]	= 0.0;

		$eva_arr['pc'][$pdate]	= $pc;
		$eva_arr['ac'][$pdate]	= 0.0;
		$eva_arr['ev'][$pdate]	= 0.0;

		$eva_arr['sv'][$pdate]	= 0.0;
		$eva_arr['cv'][$pdate]	= 0.0;
		$eva_arr['spi'][$pdate]	= 0.0;
		$eva_arr['cpi'][$pdate]	= 0.0;
	}

	// Istkosten (= Actual Cost "AC")
	// Istkosten bereits abgeschlossener Arbeit "IKAA" (= Actual Cost of Work Performed "ACWP")
	// ...Zeitdaten
	$sql  = "SELECT ist_beginn, projekt_id, kosten_ek, kosten_vk, rk_ek, rk_vk, zeitaufwand, nicht_abrechenbar, rk_nicht_abrechenbar";
	$sql .= " FROM zeitdaten WHERE mandanten_id=".$mid." AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.",".FXP_TE_TRAVELEXP.") AND ist_beginn IS NOT NULL";
	$sql .= " AND projekt_id IN (SELECT projekt_id FROM projekte WHERE vorgangsnummer LIKE '".$top_vn."%' AND mandanten_id=".$mid." AND projektart=".FXP_PRJ_TASK.")";
	$sql .= " ORDER BY ist_beginn";
	$tmp =db_values($sql, '*');
//fxDebug($tmp, $sql);
	if(fxIsArray($tmp))
	{
		foreach($eva_arr['pc'] as $pdate => $dummy)
		{
			$ae=0.0;
			$ac=0.0;

			foreach($tmp as $t)
			{
				$tpid=(int)$t['projekt_id'];
				if(isset($time_arr[$tpid]))
				{
					$tdat=substr($t['ist_beginn'], 0, 8);
					// Vergleichsdatum >= Eintragsdatum
					if($pdate >= $tdat)
					{
						$ae += get_double($t['zeitaufwand']);
						$ac += get_double($t['kosten_ek']) + get_double($t['rk_ek']);
					}
				}
			}

			$eva_arr['ae'][$pdate] += $ae;
			$eva_arr['ac'][$pdate] += $ac;
		}
	}
	// ...Sofortmaterial
	$sql  = "SELECT soll_beginn, soll_ende, fuervorgang, soll_kosten, anschaffungspreis, verkaufspreis, sachmenge, sachmittelstatus";
	$sql .= " FROM sachmittel";
	$sql .= " WHERE mandanten_id=".$mid." AND verwendungsart=3149";
	$sql .= " AND fuervorgang IN (SELECT projekt_id FROM projekte WHERE vorgangsnummer LIKE '".$top_vn."%' AND mandanten_id=".$mid." AND projektart=".FXP_PRJ_TASK.")";
	$sql .= " ORDER BY soll_beginn";
	$tmp =db_values($sql, '*');
//fxDebug($tmp, $sql);
	if(fxIsArray($tmp))
	{
		foreach($eva_arr['pc'] as $pdate => $dummy)
		{
			$ac=0.0;
			foreach($tmp as $t)
			{
				$tpid=(int)$t['fuervorgang'];
				if(isset($time_arr[$tpid]))
				{
					$tdat=substr($t['soll_beginn'], 0, 8);
					// Vergleichsdatum >= Eintragsdatum
					if($pdate >= $tdat)
					{
						// Int. verpl. Material: Summe der Planungskosten aller "bestellten" Materialien
						$im_verpl=0.0;
						if(($t['sachmittelstatus'] == 1238) && ($t['soll_kosten'] > 0.0) && ($t['sachmenge'] > 0))
							$im_verpl=get_double($t['soll_kosten'])*get_double($t['sachmenge']);
						// Int. verbr Material: Summe der Anschaffungskosten aller "bezahlten" Materialien
						$im_verbr=0.0;
						if(($t['sachmittelstatus'] == 1237) && ($t['anschaffungspreis'] > 0.0) && ($t['sachmenge'] > 0))
							$im_verbr=get_double($t['anschaffungspreis'])*get_double($t['sachmenge']);
						// Ext. verbr Material: Summe der Verkaufspreise aller "bezahlten" abrechenbaren Materialien
						$em_verbr=0.0;
						if(($t['sachmittelstatus'] == 1237) && strlen($t['soll_ende']) && ($t['verkaufspreis'] > 0.0) && ($t['sachmenge'] > 0))
							$em_verbr=get_double($t['verkaufspreis'])*get_double($t['sachmenge']);

						$ac += $im_verbr;
					}
				}
			}

			$eva_arr['ac'][$pdate] += $ac;
		}
	}

	// Tendenzen ermitteln für zukünftige Datumsfelder
	if($max_date > $today)
	{
		$last_ae=false;
		$last_ac=false;
		$tend_ae=0;
		$tend_ac=0.0;
		$date_cn=0;
		foreach($eva_arr['pc'] as $pdate => $pc)
		{
			if($pdate < $today)
			{
				if($last_ae !== false)
				{
					$tend_ae += ($eva_arr['ae'][$pdate] - $last_ae);
					$tend_ac += ($eva_arr['ac'][$pdate] - $last_ac);
					$date_cn++;
				}
				$last_ae=$eva_arr['ae'][$pdate];
				$last_ac=$eva_arr['ac'][$pdate];
			}
			else if(($pdate == $today) && ($date_cn > 0))
			{
				$tend_ae /= $date_cn;
				$tend_ac /= $date_cn;
				$date_cn=0;
			}
			else
			{
				$date_cn++;
				$eva_arr['ae'][$pdate] += $tend_ae*$date_cn;
				$eva_arr['ac'][$pdate] += $tend_ac*$date_cn;
			}
		}
	}

	// Leistungswert (= Earned Value "EV")
	// Soll-Kosten bereits abgeschlossener Arbeit "SKAA" (= Budgeted Cost of Work Performed "BCWP")
	foreach($eva_arr['pc'] as $pdate => $pc)
	{
		$percent=0.0;
		if($eva_arr['pe'][$pdate] > 0.0)
			$percent=round($eva_arr['ae'][$pdate]/$eva_arr['pe'][$pdate], 4);
		$eva_arr['pa'][$pdate]=$percent;

		$ev=$percent*$pc;
//echo('<b>'.$pdate.'</b>: $pc='.$pc.', $percent='.$percent.' -&gt; $ev='.$ev.'<br />');

		$eva_arr['ev'][$pdate]=$ev;
	}

	// Planabweichung (= Schedule Variance "SV")
	// Kostenabweichung (= Cost Variance "CV")
	// Zeiteffizienz (= Schedule Performance Index "SPI")
	// Kosteneffizienz (= Cost Performance Index "CPI")
	foreach($eva_arr['pc'] as $pdate => $pc)
	{
		// Planabweichung (= Schedule Variance "SV")
		//=Leistungswert "EV" - Plankosten "PC"
		$eva_arr['sv'][$pdate]=$eva_arr['ev'][$pdate] - $eva_arr['pc'][$pdate];

		// Kostenabweichung (= Cost Variance "CV")
		//=Leistungswert "EV" - Istkosten "AC"
		$eva_arr['cv'][$pdate]=$eva_arr['ev'][$pdate] - $eva_arr['ac'][$pdate];

		// Zeiteffizienz (= Schedule Performance Index "SPI")
		//=Leistungswert "EV" / Plankosten "PC"
		if($eva_arr['pc'][$pdate] > 0.0)
			$eva_arr['spi'][$pdate]=round($eva_arr['ev'][$pdate]/$eva_arr['pc'][$pdate], 2);

		// Kosteneffizienz (= Cost Performance Index "CPI")
		//=Leistungswert "EV" / Istkosten "AC"
		if($eva_arr['ac'][$pdate] > 0.0)
			$eva_arr['cpi'][$pdate]=round($eva_arr['ev'][$pdate]/$eva_arr['ac'][$pdate], 2);
	}

//fxDebug($eva_arr, '$eva_arr');
	return $eva_arr;
}

/**
 * ???FUNCTION???
 *
 * @param various $day         - Mandatory parameter:                  ???PARAMETER???
 * @param various $start_date  - Mandatory parameter:                  ???PARAMETER???
 * @param string  $end_date    - Optional parameter (default = ''):    ???PARAMETER???
 * @param boolean $split_month - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getQuarter($day, $start_date, $end_date='', $split_month=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$id='';
	$day=substr($day, 0, 8);
	$start_date=substr($start_date, 0, 8);
	if(strlen($end_date))
		$end_date=substr($end_date, 0, 8);

	$valid=true;
	if(($day < $start_date) || (strlen($end_date) && ($day > $end_date)))
		$valid=false;

	if($valid)
	{
		// Zeiten in Quartale aufsplitten
		$quart_sy=(int)substr($start_date, 0, 4);
		$quart_sm=(int)substr($start_date, 4, 2);
		$quart_sd=(int)substr($start_date, 6, 2);

		while(true)
		{
			if($quart_sd == 1)
			{
				$quart_em=$quart_sm+2;
				$quart_ed=31;
			}
			else
			{
				$quart_em=$quart_sm+3;
				$quart_ed=$quart_sd-1;
			}
			$quart_ey=$quart_sy;
			if($quart_em > 12)
			{
				$quart_em -= 12;
				$quart_ey++;
			}

			$quart_start=$quart_sy.substr('00'.$quart_sm, -2).substr('00'.$quart_sd, -2);
			$quart_end  =$quart_ey.substr('00'.$quart_em, -2).substr('00'.$quart_ed, -2);
//echo($day.' -- Quarter: '.$quart_start.' - '.$quart_end.'<br />');

			if(($day >= $quart_start) && ($day <= $quart_end))
			{
//echo('<b>Quarter found</b>...<br />');
				if($split_month && ($quart_sd > 1))
				{
					$q=substr($quart_end, 0, 6).'01';
					if($day >= $q)
						$quart_start=$q;
					else
					{
						$quart_em--;
						if($quart_em < 1)
						{
							$quart_em=12;
							$quart_ey--;
						}
						$quart_end=$quart_ey.substr('00'.$quart_em, -2).'31';
					}
				}
				if(strlen($end_date))
					$quart_end=min($quart_end, $end_date);
				$quart_start=fx_date('Ymd', fx_mktime(12,0,0, (int)substr($quart_start, 4, 2),(int)substr($quart_start, 6, 2),(int)substr($quart_start, 0, 4)));
				$quart_end=min($quart_end, fx_date('Ymd', fx_mktime(12,0,0, 1+(int)substr($quart_end, 4, 2),0,(int)substr($quart_end, 0, 4))));
				$id=$quart_start.'-'.$quart_end;
//echo('-&gt; $id='.$id.'<br />');
				break;
			}

			// Nächstes Quartal
			$quart_sm += 3;
			if($quart_sm > 12)
			{
				$quart_sm -= 12;
				$quart_sy++;
			}
		}
	}

	return $id;
}

/**
 * Funktion zum Validieren von Projektparametern
 *
 * @param various $attr_arr - Mandatory parameter:                ???PARAMETER???
 * @param string  $mnr      - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function validateProjectAttributes($attr_arr, $mnr='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!fxIsArray($attr_arr))
		return true;

	if(!strlen($mnr))
		$mnr=$GLOBALS['_masknumber'];

	// Validierungstypen
	$tarr=array('t'=>"text", 'd'=>"datum", 's'=>"auswahl", 'i'=>"ganzzahl", 'f'=>"dezimal", 'c'=>"ja/nein", 'z'=>"zeit", 'a'=>"zeitspanne");

	$verr=array();
	foreach($attr_arr as $attr)
	{
		$attr	= explode('|', $attr);
		$type	= strtolower(substr($attr[0], 0, 1));	// Typ: t=Text, d=Datum, s=Select, i=Integer, f=Float, c=Checkbox, z=Zeit, a=Zeitspanne
		$fid	= (int)$attr[1];						// Felder-ID
		$fname	= trim($attr[2]);						// Feldname
		$mand	= (int)$attr[3];						// Mussfeld

		$value	= $_POST[$fname];

		$terr=false;
		$_POST[$fname]=validate_entry($value, $terr, $tarr[$type], $mand);
//echo($_POST[$fname].'=validate_entry('.$value.', $terr, '.$tarr[$type].', '.$mand.');<br />');
		if($terr)
		{
			// Add field to mask if not found
			addMaskField($fid, $mnr);

			error_msg($terr, false, $fname);
			$verr[$fname]=$terr;
		}
	}

	return $verr;
}

/**
 * Funktion generiert bzw. liest für einen Vorgang einen eindeutigen PCA-Code
 *
 * @param various $pid - Mandatory parameter:                ???PARAMETER???
 * @param string  $def - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dummy_pca_code($pid, $def='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pid=(int)$pid;
	if(!$pid)
		return 1805;	// Es wurde keine Projekt-ID übermittelt oder die Projekt-ID ist 0!

	$mid=$GLOBALS['fxpglobals']['client'];

	// Projektinfos
	$pex=false;
	$sql="SELECT vorgangsnummer FROM projekte WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
	$inf=db_values($sql);
//fxDebug($inf, $sql);
	if(is_array($inf))
	{
		$vn =no_null($inf['vorgangsnummer']);
		$pex=true;
	}

	// Fehlervariable
	$pca_err=false;

	// PCA-Code ermitteln
	$sql="SELECT parameter_txt FROM projekte_parameter WHERE projekt_id=".$pid." AND mandanten_id=".$mid." AND parameter_typ=376 AND parameter_var='TXT'";
	$pca=trim(db_value($sql));
//fxDebug($pca, $sql);
	// ...vorhanden, aber kein Vorgang mehr, dann löschen
	if(strlen($pca) && !$pex)
	{
		$sql="DELETE FROM projekte_parameter WHERE projekt_id=".$pid." AND mandanten_id=".$mid." AND parameter_typ=376 AND parameter_var='TXT'";
		$pca_err=db_x($sql);
	}
	// ...nicht vorhanden, aber Vorgang, dann neu generieren
	else if(!strlen($pca) && $pex)
	{
		if(strlen($def))
			$ndbpca="'".convert_string($def, 'todb')."'";
		else
		{
			// Platzhalter
			$pca_platzhalter=trim($GLOBALS['fxpglobals']['settings']['pca']);
			$pca_ad=0;
			$pca_pa='';
			$pca_tp='';
			for($i=0; $i<strlen($pca_platzhalter); $i++)
			{
				$c=substr($pca_platzhalter, $i, 1);
				// Korrekte Zeichen
				if(($c == 'P') || ($c == 'N') || ($c == 'Y') || ($c == 'M') || ($c == 'D') || ($c == 'h') || ($c == 'm') || ($c == 's') || ($c == 't') || ($c == 'n'))
				{
					if($c == $pca_tp)
						$pca_pa[$pca_ad]['len']++;
					else
					{
						if(is_array($pca_pa))
							$pca_ad++;

						$pca_pa[$pca_ad]=array('pos'=>$i, 'len'=>1, 'typ'=>$c);
						$pca_tp=$c;
					}
				}
				else
					$pca_tp='';
			}
//fxDebug($pca_pa, $pca_platzhalter);

			$pcnt=0;
			while(true)
			{
				$ndbpca	= '';
				$pcnt++;
				// Maximal 100 Versuche erlauben um einen eindeutigen PCA-Code zu generieren
				if($pcnt > 100)
					break;

				if(!isset($GLOBALS['pca_lfdn']))
					$GLOBALS['pca_lfdn']=1;
				else
					$GLOBALS['pca_lfdn'] += 1;

				$npca=$pca_platzhalter;
				if(is_array($pca_pa))
				{
					$t=time();
					list($usec, $sec)=explode(' ', microtime());

					foreach($pca_pa as $p)
					{
						$val	= substr($npca, $p['pos'], $p['len']);
						$c		= $p['typ'];
						switch($c)
						{
							case 'P':	// Interne Projekt-ID
								$val=substr(str_repeat('0', $p['len']).$pid, -$p['len']);
							break;

							case 'N':	// Vorgangsnummer
								$val=substr($vn.str_repeat(' ', $p['len']), 0, $p['len']);
							break;

							case 'Y':	// Jahr
								$val=substr(str_repeat('0', $p['len']).fx_date('Y', $t), -$p['len']);
							break;

							case 'M':	// Monat
								$val=substr(str_repeat('0', $p['len']).fx_date('m', $t), -$p['len']);
							break;

							case 'D':	// Tag
								$val=substr(str_repeat('0', $p['len']).fx_date('d', $t), -$p['len']);
							break;

							case 'h':	// Stunde
								$val=substr(str_repeat('0', $p['len']).fx_date('H', $t), -$p['len']);
							break;

							case 'm':	// Minute
								$val=substr(str_repeat('0', $p['len']).fx_date('i', $t), -$p['len']);
							break;

							case 's':	// Sekunde
								$val=substr(str_repeat('0', $p['len']).fx_date('s', $t), -$p['len']);
							break;

							case 't':	// Tausendstel-Sekunde
								$val=substr(substr($usec, 2).str_repeat('0', $p['len']), 0, $p['len']);
							break;

							case 'n':	// Fortlaufende Nummer
								$val=substr(str_repeat('0', $p['len']).$GLOBALS['pca_lfdn'], -$p['len']);
							break;
						}

						$npca=substr($npca, 0, $p['pos']).$val.substr($npca, $p['pos']+$p['len']);
					}
				}

				// Eindeutigkeit prüfen
				$npca=str_replace(' ', '', $npca);
				$ndbpca="'".convert_string($npca, 'todb')."'";
				$sql="SELECT parameter_id FROM projekte_parameter WHERE mandanten_id=".$mid." AND parameter_typ=376 AND parameter_var='TXT' AND parameter_txt=".$ndbpca;
				$chk=(int)db_value($sql);
//fxDebug($chk, $sql);
				// ...nicht gefunden, dann anlegen
				if(!$chk)
					break;
			}
		}
//fxDebug($npca, $pcn);

		// Neuen PCA-Code in DB schreiben
		if(strlen($ndbpca))
		{
	 		$paid=id_max('projekte_parameter', 'parameter_id');
			$sql  = "INSERT INTO projekte_parameter";
			$sql .= " (mandanten_id, parameter_id, projekt_id, parameter_typ, parameter_var, parameter_int, parameter_dec, parameter_txt, anlage_dtm, angelegt_von) ";
			$sql .= "VALUES";
			$sql .= " (".$mid.", ".$paid.", ".$pid.", 376, 'TXT', NULL, NULL, ".$ndbpca.", '".$GLOBALS['datetime']."', ".$GLOBALS['fxpglobals']['person'].")";

			$pca_err=db_x($sql, 0);
//fxDebug($sql, 'ERR='.$pca_err);
			if(!$pca_err)
				$pca=$npca;
	 	}
	 	else
	 		$pca_err=true;
	}

	// PCA-Code zurückliefern
	if($pca_err)
		return '?';
	else
		return $pca;
}

/**
 * ???FUNCTION???
 *
 * @param various $feldname - Mandatory parameter:               ???PARAMETER???
 * @param various $literal  - Mandatory parameter:               ???PARAMETER???
 * @param integer $id       - Optional parameter (default = 0):  ???PARAMETER???
 */
function add_field($feldname, $literal, $id=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!is_array($GLOBALS['fx_mfields']) && !is_array($GLOBALS['fx_mfields']['425']))
		return;

	// Neuen Schlüsselwert ermitteln
	$new_key_literal	= sizeof($GLOBALS['fx_mfields']['425']);
	$new_key_select		= $new_key_literal + 1;

	// Neuen Spaltenindex + -nummer ermitteln
	$new_key_column		= sizeof($GLOBALS['fx_mfieldsarray']['425']['columns']);
	$new_column			= (string)(10 + $GLOBALS['fx_mfieldsarray']['425']['columns'][$new_key_column - 1]);

	// Neue Feld-ID
	$id=(int)$id;
	if(strlen($id) == 0 || $id == 0)
		$id=25000 + $new_key_literal;

	// Zeile 1 und 10: Literal + Auswahlfeld vom Projektleiterfeld kopieren und anpassen
	for($i=0; $i < 2; $i++)
	{
		if($i == 0)	// Zeile 1
		{
			$new_key	= $new_key_literal;
			$source_id	= 7;
		}
		else	// Zeile 10
		{
			$new_key	= $new_key_select;
			$source_id	= 55;
		}

		$GLOBALS['fx_mfields']['425'][$new_key]=$GLOBALS['fx_mfields']['425'][$source_id];

		$GLOBALS['fx_mfields']['425'][$new_key]['maskenfeldid']		= (string)(50000+$new_key);
		$GLOBALS['fx_mfields']['425'][$new_key]['spalte']			= $new_column;
		$GLOBALS['fx_mfields']['425'][$new_key]['eingabefeld']		= (string)$id;
		$GLOBALS['fx_mfields']['425'][$new_key]['feldname']			= $feldname;
		$GLOBALS['fx_mfields']['425'][$new_key]['id']				= (string)$id;
		$GLOBALS['fx_mfields']['425'][$new_key]['muss']				= '0';
		$GLOBALS['fx_mfields']['425'][$new_key]['literal_value']	= $literal;
		$GLOBALS['fx_mfields']['425'][$new_key]['literal_5']		= substr($literal, 0, 5);
		$GLOBALS['fx_mfields']['425'][$new_key]['literal_10']		= substr($literal, 0, 10);
		$GLOBALS['fx_mfields']['425'][$new_key]['literal_15']		= substr($literal, 0, 15);
		$GLOBALS['fx_mfields']['425'][$new_key]['literal_20']		= substr($literal, 0, 20);
	}

	// Neuen Index + Größen anpassen
	$GLOBALS['fx_mfieldsarray']['425']['columns'][(string)$new_key_column]	= $new_column;
	$GLOBALS['fx_mfieldsarray']['425']['width'][$new_column]				= '0';
	$GLOBALS['fx_mfieldsarray']['425']['1'][$new_column]					= 'blank';
	$GLOBALS['fx_mfieldsarray']['425']['10'][$new_column]					= 'blank';
	$GLOBALS['fx_mfieldsarray']['425']['9999'][$new_column]					= 'blank';
}

/**
 * ???FUNCTION???
 *
 * @return ???RETURN???
 */
function get_resp_roles()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// vorhandene Zuständigkeitsfähigkeiten-Personen-Zuordnungen ermitteln
	$sql  = "SELECT mf.mafaehigkeit_name, maf.mafaehigkeit_id, p.personen_id, p.pname, p.vorname, p.inaktiv";
	$sql .= " FROM ma_faehigkeit mf, ma_art_faehigkeiten maf, personen p";
	$sql .= " WHERE maf.personen_id=p.personen_id AND mf.mafaehigkeit_id=maf.mafaehigkeit_id AND maf.maart_id=2";
	$sql .= " AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND maf.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND mf.mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].") AND mf.id_sprache=".$GLOBALS['fxpglobals']['lang'];
	$sql .= " ORDER BY mf.mafaehigkeit_name, p.pname, p.vorname";
	$resp_roles=db_values($sql, '*');
	if(is_array($resp_roles))
	{
		foreach($resp_roles as $cnt => $resp_role)
			$roles['pvr_'.$resp_role['mafaehigkeit_id']][]=$resp_role;
	}
	return $roles;
}

/**
 * ???FUNCTION???
 *
 * @return ???RETURN???
 */
function get_resp_role_names()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Vorhandene Zuständigkeitsfähigkeiten-Personen-Zuordnungen ermitteln
	$sql  = "SELECT DISTINCT mf.mafaehigkeit_name, maf.mafaehigkeit_id";
	$sql .= " FROM ma_faehigkeit mf, ma_art_faehigkeiten maf, personen p";
	$sql .= " WHERE maf.personen_id=p.personen_id AND mf.mafaehigkeit_id=maf.mafaehigkeit_id AND maf.maart_id=2";
	$sql .= " AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND maf.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND mf.mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].") AND mf.id_sprache=".$GLOBALS['fxpglobals']['lang'];
	$sql .= " ORDER BY mf.mafaehigkeit_name";
	$roles=db_values($sql, '*');
//fxDebug($roles, 'get_resp_role_names');
	return $roles;
}

/**
 * ???FUNCTION???
 *
 * @param various $resp_roles      - Mandatory parameter: ???PARAMETER???
 * @param various $mask_cnt        - Mandatory parameter: ???PARAMETER???
 * @param various $line            - Mandatory parameter: ???PARAMETER???
 * @param various &$selected_value - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function build_resp_role_selects($resp_roles, $mask_cnt, $line, &$selected_value)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	//fxDebug($selected_value, '$selected_value');
//fxDebug($resp_roles, '$resp_roles');
	if(is_array($resp_roles))
	{
		$empty_msg=meldung(794, false);
		// ... Optionen der jeweiligen Selectfelder erstellen ...
		foreach($resp_roles as $feldname => $rolle)
		{
			foreach($rolle as $cnt => $mitarbeiter)
			{
				$selected='';
				if($selected_value[$feldname] == $mitarbeiter['personen_id'])
					$selected=' SELECTED';
				$selection[$feldname] .= '<OPTION'.$selected.' VALUE="'.$mitarbeiter['personen_id'].'">'.$mitarbeiter['pname'].', '.$mitarbeiter['vorname'].'</OPTION>';
			}
		}
		// ... und zu ganzen Selectfeldern kombinieren
		foreach($selection as $feldname => $select)
		{
			$empty_selected='';
			if(strpos($selection[$feldname], '<OPTION SELECTED VALUE=') === false)
				$empty_selected=' SELECTED';
			$id=$feldname.'['.$mask_cnt.']['.$line.']';
			$selection[$feldname]='<SELECT ID="'.$id.'" NAME="'.$id.'"><OPTION'.$empty_selected.' VALUE="0">'.$empty_msg.'</OPTION>'.$selection[$feldname].'</SELECT>';
		}
//fxDebug($selection, '$selection');
	}
	// ... und als Array zurückgeben
	return $selection;
}

/**
 * ???FUNCTION???
 */
function matrixSetFileDividers()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$GLOBALS['ds0']='*|*';
	$GLOBALS['ds1']='*#*';
	$GLOBALS['ds2']='*~*';
	$GLOBALS['ds3']='*<>*';
}

/**
 * ???FUNCTION???
 *
 * @param various $pid      - Mandatory parameter:                  ???PARAMETER???
 * @param various $swidth   - Optional parameter (default = -1):    ???PARAMETER???
 * @param various $dwidth   - Optional parameter (default = -1):    ???PARAMETER???
 * @param various $gwidth   - Optional parameter (default = -1):    ???PARAMETER???
 * @param integer $rpheight - Optional parameter (default = 0):     ???PARAMETER???
 * @param boolean $prefix   - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function matrixFile($pid,$swidth=-1,$dwidth=-1,$gwidth=-1,$rpheight=0,$prefix=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	matrixSetFileDividers();

	getProjectUserFields();

	// "Freigabe" (= approval)
	$app=prjTableHasApproval();

	if($prefix === true)
		$prefix=get_prefix_project($pid);
	$pfl=strlen($prefix);

	$open_project_limit=128;

	$sql="SELECT mutterprojekt_id, vorgangsnummer FROM projekte WHERE projekt_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$inf=db_values($sql);
	$parr=array();	// ...projects
	$darr=array();	// ...dependencies
	if(fxIsArray($inf))
	{
		$mp=$inf['mutterprojekt_id'];
		$pn=$inf['vorgangsnummer'];
		$mpa=projektinfo($mp);
		if(fxIsArray($mpa))
		{
			// Get userfields
			$uarr=array();
			if(fxIsArray($GLOBALS['ufields']) && isset($GLOBALS['utable']) && strlen($GLOBALS['utable']))
			{
				$sql="SELECT * FROM ".$GLOBALS['utable']." WHERE projekt_id IN (SELECT projekt_id FROM projekte WHERE mutterprojekt_id=".$mp." AND mandanten_id=".$GLOBALS['fxpglobals']['client'].") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$arr=db_values($sql,'*');
				if(fxIsArray($arr))
				{
					foreach($arr as $a)
					{
						$uid=(int)$a['projekt_id'];
						$uarr[$uid]=array();
						foreach($GLOBALS['ufields'] as $fid => $fdat)
						{
							$ufn=strtolower($fdat['feldname']);
							$uarr[$uid][$fid]=$a[$ufn];
						}
					}
				}
			}
//fxDebug($uarr, '$uarr');

			// Get milestones
			$sql  = "SELECT meilenstein_id,projekt_id,name_meilenstein,soll_ende,statusfeld,tagesanzahl FROM meilensteine WHERE mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$sql .= " AND projekt_id IN (SELECT projekt_id FROM projekte WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND mutterprojekt_id=".$mp.")";
			$marr=db_values($sql,'*');

			// Get dependencies
			$sql  = "SELECT projekt_a,projekt_b,abh_typ,tagesanzahl FROM abhaengigkeiten WHERE mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$sql .= " AND projekt_a IN (SELECT projekt_id FROM projekte WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND mutterprojekt_id=".$mp.")";
			$dta=db_values($sql,'*');
			if(fxIsArray($dta))
			{
				foreach($dta as $d)
					$darr[$d['projekt_a']]=$d;
			}

			// Get project rights
			$rarr=array();
			if(!$GLOBALS['fxpglobals']['persdat']['roles'][2])	// No GM
			{
				foreach($mpa as $ppid => $pdat)
					$rarr[$pdat['data']['vorgangsnummer']]=0;

				// ...User
				$sql  = "SELECT p.vorgangsnummer, r.aktionscode FROM rechte_referenz r, projekte p";
				$sql .= " WHERE r.rechte_ref=37 AND r.gruppeart=2163 AND r.benutzergr_id=".$GLOBALS['fxpglobals']['user']." AND r.mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$sql .= " AND r.id_referenz IN (SELECT projekt_id FROM projekte WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND mutterprojekt_id=".$mp.")";
				$sql .= " AND p.projekt_id=r.id_referenz AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
				if(fxIsArray($tmp))
				{
					foreach($tmp as $t)
					{
						$tvnl=strlen($t['vorgangsnummer']);
						foreach($mpa as $ppid => $pdat)
						{
							if(substr($pdat['data']['vorgangsnummer'],0,$tvnl) == $t['vorgangsnummer'])
								$rarr[$pdat['data']['vorgangsnummer']] |= $t['aktionscode'];
						}
					}
				}
				// ...User groups
				$uga=get_benutzergruppen($GLOBALS['fxpglobals']['user']);
				if(fxIsArray($uga))
				{
					$ugc=array_to_csv($uga);
					$sql  = "SELECT p.vorgangsnummer, r.aktionscode FROM rechte_referenz r, projekte p";
					$sql .= " WHERE r.rechte_ref=37 AND r.gruppeart=2164 AND r.benutzergr_id IN (".$ugc.") AND r.mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$sql .= " AND r.id_referenz IN (SELECT projekt_id FROM projekte WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND mutterprojekt_id=".$mp.")";
					$sql .= " AND p.projekt_id=r.id_referenz AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$tmp=db_values($sql, '*');
//fxDebug($tmp,$sql);
					if(fxIsArray($tmp))
					{
						foreach($tmp as $t)
						{
							$tvnl=strlen($t['vorgangsnummer']);
							foreach($mpa as $ppid => $pdat)
							{
								if(substr($pdat['data']['vorgangsnummer'],0,$tvnl) == $t['vorgangsnummer'])
									$rarr[$pdat['data']['vorgangsnummer']] |= $t['aktionscode'];
							}
						}
					}
				}
				// ...Department
				$dpa=get_abteilungen($GLOBALS['fxpglobals']['person']);
				if(fxIsArray($dpa))
				{
					$dpc=array_to_csv($dpa);
					$sql  = "SELECT p.vorgangsnummer, r.aktionscode FROM rechte_referenz r, projekte p";
					$sql .= " WHERE r.rechte_ref=37 AND r.gruppeart=2165 AND r.benutzergr_id IN (".$dpc.") AND r.mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$sql .= " AND r.id_referenz IN (SELECT projekt_id FROM projekte WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND mutterprojekt_id=".$mp.")";
					$sql .= " AND p.projekt_id=r.id_referenz AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$tmp=db_values($sql, '*');
//fxDebug($tmp,$sql);
					if(fxIsArray($tmp))
					{
						foreach($tmp as $t)
						{
							$tvnl=strlen($t['vorgangsnummer']);
							foreach($mpa as $ppid => $pdat)
							{
								if(substr($pdat['data']['vorgangsnummer'],0,$tvnl) == $t['vorgangsnummer'])
									$rarr[$pdat['data']['vorgangsnummer']] |= $t['aktionscode'];
							}
						}
					}
				}
//fxDebug($rarr,'$rarr');
			}

			// Setting [pm_aendert_mp]: "Master projects can only be changed by the assigned project manager (except main administrator or general management)?"
			$only_pm=false;
			if(strtolower($GLOBALS['fxpglobals']['settings']['pm_aendert_mp']) == 'ja')
				$only_pm=true;

			$ccp=0;
			$cst=0;
			$cca=array();
			if($GLOBALS['fxpglobals']['persdat']['personentyp'] == FXP_PT_CUSTOMER_CP)
			{
				$ccp=$GLOBALS['fxpglobals']['person'];
				$cst=$GLOBALS['fxpglobals']['persdat']['elter'];

				$sql  = "SELECT DISTINCT(p.projekt_id) FROM projekte_history p, budget_summe_history b";
				$sql .= " WHERE b.projekt_id=p.projekt_id AND b.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND p.mutterprojekt_id=".$mp." AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$sql .= " AND (p.aenderungs_id<>".$ccp." OR b.aenderungs_id<>".$ccp.")";
				$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
				if(fxIsArray($tmp))
				{
					foreach($tmp as $t)
						$cca[$t['projekt_id']]=true;
				}
			}
//fxDebug($cca,'$cca');

			$hr=false;
			if(tr_rechte(43))
				$hr=true;

			$ln=strlen($pn);
			foreach($mpa as $ppid => $pdat)
			{
//fxDebug($pdat,'$pdat: $ppid='.$ppid);
				$valid=false;
				if(substr($pdat['data']['vorgangsnummer'],0,$ln) == $pn)
					$valid=true;

				$disp=true;
				if(($pdat['data']['projektart'] == FXP_PRJ_TASK) && !$valid)
					$disp=false;

				if($disp)
				{
					// Rights:
					// ...Bit 0= 1:	Valid
					// ...Bit 1= 2:	Display
					// ...Bit 2= 4:	Create
					// ...Bit 3= 8:	Edit
					// ...Bit 4=16:	Delete
					// ...Bit 5=32:	Move
					// ...Bit 6=64: HR
					$rights=0;
					if($valid)
					{
						$rights=1;

						if(($pdat['data']['projektmanager'] == $GLOBALS['fxpglobals']['person']) || $GLOBALS['fxpglobals']['persdat']['roles'][2])	// PM or GM
						{
							if($ppid == $mp)	// Master project?
								$rights=47;		// + Display:2 + Create:4 + Edit:8 + Move:32
							else
								$rights=63;		// + Display:2 + Create:4 + Edit:8 + Delete:16 + Move:32
						}
						else if($ppid == $mp)	// Master project?
						{
							if(($pdat['data']['angelegt_von'] == $GLOBALS['fxpglobals']['person']) || ($pdat['data']['kunde'] == $cst) || ($pdat['data']['projektleiter'] == $GLOBALS['fxpglobals']['person']) || ($pdat['data']['vertreter'] == $GLOBALS['fxpglobals']['person']))	// Creator, Customer, PL or DPL
							{
								if($only_pm || ($pdat['data']['kunde'] == $cst))
									$rights=7;	// + Display:2 + Create:4
								else
									$rights=47;	// + Display:2 + Create:4 + Edit:8 + Move:32
							}
						}
						else if($ppid == $pid)	// Selected subproject?
						{
							if($pdat['data']['kunde'] == $cst)
								$rights=7;	// + Display:2 + Create:4
							else
								$rights=15;	// + Display:2 + Create:4 + Edit:8
						}
						else if($pdat['data']['kunde'] == $cst)	// Customer
						{
							if($pdat['data']['ansprechpartner'] == $ccp)	// Contact
							{
								if($cca[$ppid])
									$rights=103;	// + Display:2 + Create:4 + Move:32 + HR:64
								else
									$rights=127;	// + Display:2 + Create:4 + Edit:8 + Delete:16 + Move:32 + HR:64
							}
							else
								$rights=7;	// + Display:2 + Create:4
						}
						else if(($pdat['data']['angelegt_von'] == $GLOBALS['fxpglobals']['person']) || ($pdat['data']['projektleiter'] == $GLOBALS['fxpglobals']['person']) || ($pdat['data']['vertreter'] == $GLOBALS['fxpglobals']['person']))	// Creator, PL or DPL
							$rights=63;	// + Display:2 + Create:4 + Edit:8 + Delete:16 + Move:32

						// Enable HR?
						if($hr && !($rights&64) && ($ppid != $mp) && ($ppid != $pid) && ($GLOBALS['fxpglobals']['persdat']['roles'][2] || $GLOBALS['fxpglobals']['persdat']['roles'][-3] || ($pdat['data']['angelegt_von'] == $GLOBALS['fxpglobals']['person']) || ($pdat['data']['projektleiter'] == $GLOBALS['fxpglobals']['person']) || ($pdat['data']['vertreter'] == $GLOBALS['fxpglobals']['person'])))	// GM, RP, Creator, PL or DPL
							$rights += 64;

						// Disable HR?
						if(!$hr && $rights&64)
							$rights -= 64;

						// Assign user rights
						if(($rights == 1) && isset($rarr[$pdat['data']['vorgangsnummer']]) && ($rarr[$pdat['data']['vorgangsnummer']] > 0))
						{
							$rights |= $rarr[$pdat['data']['vorgangsnummer']];
							if($rights & 8)	// Edit?
								$rights |= 96;	// then also Move:32 + HR:64
							if($ppid == $mp)	// Master project?
							{
								if($only_pm || ($pdat['data']['kunde'] == $cst))
									$rights &= 7;	// + Display:2 + Create:4
								else
									$rights &= 47;	// + Display:2 + Create:4 + Edit:8 + Move:32
							}
							else if($ppid == $pid)	// Selected subproject?
							{
								if($pdat['data']['kunde'] == $cst)
									$rights &= 7;	// + Display:2 + Create:4
								else
									$rights &= 15;	// + Display:2 + Create:4 + Edit:8
							}
						}
					}
					$parr[$ppid]=$pdat;
					$parr[$ppid]['rechte']=$rights;
					$parr[$ppid]['folder']='fc';
					$parr[$ppid]['hidden']=0;
					$parr[$ppid]['userfields']=array();
					if(fxIsArray($uarr[$ppid]))
						$parr[$ppid]['userfields']=$uarr[$ppid];
//echo($ppid.' ($pid='.$pid.', $mp='.$mp.'): '.no_null($pdat['data']['vorgangsnummer']).' '.$pdat['data']['name_projekt'].' -- $rights='.$rights.'<br />');

					// Milestones
					if($rights && fxIsArray($marr))
					{
						foreach($marr as $m)
						{
							if($m['projekt_id'] == $ppid)
							{
								$mpid=-$m['meilenstein_id'];
								$parr[$mpid]=$pdat;
								$parr[$mpid]['rechte']=$rights;
								$parr[$mpid]['folder']='fc';
								$parr[$mpid]['hidden']=0;
								$parr[$mpid]['userfields']=array();

								$mstn=$m['name_meilenstein'];
								if(!strlen($mstn))
									$mstn='MST-'.$m['meilenstein_id'];

								$parr[$mpid]['data']['projekt_id']		= $m['meilenstein_id'];
								$parr[$mpid]['data']['vorgangsnummer']	= $pdat['data']['vorgangsnummer'].'.0000';
								$parr[$mpid]['data']['name_projekt']	= $mstn;
								$parr[$mpid]['data']['soll_beginn']		= $m['soll_ende'];
								$parr[$mpid]['data']['soll_ende']		= $m['soll_ende'];
								$parr[$mpid]['data']['projekt_status']	= $m['statusfeld'];
								$parr[$mpid]['data']['maart_id']		= $m['tagesanzahl'];
							}
						}
					}
				}
			}
		}
	}

	$txt='';
	if(fxIsArray($parr))
	{
		$bfields=array(
			'ihrframe'=>"budgetrahmen_pers_int", 'ihrplan'=>"budget_pers_int_gepl", 'ihrappr'=>"budget_pers_int_gen", 'ihrused'=>"budget_pers_int_verbr",
			'ehrframe'=>"budgetrahmen_pers_ext", 'ehrplan'=>"budget_pers_ext_gepl", 'ehrappr'=>"budget_pers_ext_gen", 'ehrused'=>"budget_pers_ext_verbr", 'ehrinv'=>"budget_pers_ext_abger",
			'imrframe'=>"budgetrahmen_mat_int", 'imrplan'=>"budget_mat_int_gepl", 'imrappr'=>"budget_mat_int_gen", 'imrused'=>"budget_mat_int_verbr",
			'emrframe'=>"budgetrahmen_mat_ext", 'emrplan'=>"budget_mat_ext_gepl", 'emrappr'=>"budget_mat_ext_gen", 'emrused'=>"budget_mat_ext_verbr", 'emrinv'=>"budget_mat_ext_abger"
		);

		// Open folder?
		$def_folder='fo';
		if(sizeof($parr) > $open_project_limit)
			$def_folder='fc';
//fxDebug($parr,'$parr');
		foreach($parr as $id => $dat)
		{
			$rm=0;
			if(($id < 0) || ($dat['data']['projektart'] == FXP_PRJ_TASK))
				$rm=1;
			else if($dat['rechte'] > 1)
				$rm=2;

			if($rm > 0)
			{
				if($rm == 1)
					$parr[$id]['folder']='fo';
				else
					$parr[$id]['folder']=$def_folder;

				if($dat['rechte'] > 1)
				{
					$elt=$dat['data']['elter'];
					while($elt && fxIsArray($parr[$elt]))
					{
						if($parr[$elt]['rechte'] > 1)
							$parr[$elt]['folder']=$def_folder;
						else
						{
							$parr[$elt]['rechte']=1;
							$parr[$elt]['folder']='fo';
						}
						$elt=$parr[$elt]['data']['elter'];
					}
				}
			}
		}

		// Restore previous folder attributes
		if(fxIsArray($GLOBALS['mfha']))
		{
			foreach($parr as $id => $dat)
			{
				if(fxIsArray($GLOBALS['mfha'][$id]))
				{
					$parr[$id]['folder']=$GLOBALS['mfha'][$id]['folder'];
					$parr[$id]['hidden']=$GLOBALS['mfha'][$id]['hidden'];
				}
			}
		}

		// ...Project data=Typ | ID | Nummer | Name | Startdatum | Enddatum | Aufwand-Soll | teffort | Status | Open | Hidden
		// ...Milestone data=Projektnummer | Name | Punkt | Puffer
		foreach($parr as $id => $dat)
		{
			if(strlen($txt))
				$txt .= $GLOBALS['ds1'];

			if($id < 0)
				$type='M';
			else if($dat['data']['projektart'] == FXP_PRJ_TASK)
				$type='T';
			else
				$type='P';

			// Remove prefix from project name
			$pn=$dat['data']['name_projekt'];
			if(($type != 'M') && $pfl && (substr($pn,0,$pfl) === $prefix))
				$pn=trim(substr($pn,$pfl+1));

			// Skill
			$psk=(int)$dat['data']['mafaehigkeit_id'];
			if($psk)
				$psk .= 'f';
			else
				$psk=(int)$dat['data']['maart_id'];

			$txt .= $type;															//  0: type=Type (P=Project, T=Task, M=Milestone)
			$txt .= $GLOBALS['ds0'].$dat['data']['projekt_id'];						//  1: id=ID
			$txt .= $GLOBALS['ds0'].no_null($dat['data']['vorgangsnummer']);		//  2: no=Number
			$txt .= $GLOBALS['ds0'].$pn;											//  3: name=Name
			$txt .= $GLOBALS['ds0'].substr($dat['data']['soll_beginn'],0,8);		//  4: sdate=Start date
			$txt .= $GLOBALS['ds0'].substr($dat['data']['soll_ende'],0,8);			//  5: edate=End date
			$txt .= $GLOBALS['ds0'].$dat['rechte'];									//  6: right=Rights (0=None, 1=Change values only, 2=Complete: Change values and move project
			$txt .= $GLOBALS['ds0'].$dat['data']['maart_id'];						//  7: ddiff=Milestone date diff, else ignored
			$txt .= $GLOBALS['ds0'].$dat['folder'];									//  8: open=Folder open "fo" (or closed "fc")
			$txt .= $GLOBALS['ds0'].$dat['hidden'];									//  9: hidden=Folder hidden?
//echo($dat['data']['projekt_id'].': type='.$type.', number='.$dat['data']['vorgangsnummer'].', rights='.$dat['rechte'].', folder='.$dat['folder'].'<br />');

			$di=checkDelProject($id, true, false);
			$txt .= $GLOBALS['ds0'].$di;											// 10: info=Delete info

			// ...Detail column datas: 11-x
			$txt .= $GLOBALS['ds0'].'status=I'.$dat['data']['projekt_status'];		// Status

			if($id > 0)
			{
				$txt .= $GLOBALS['ds0'].'priority=I'.$dat['data']['prioritaet'];	// Priority
				if($app)
				{
					$appv='';
					if($dat['data']['freigabe'])
						$appv='1';
					$txt .= $GLOBALS['ds0'].'app=C'.$appv;							// Approval
				}

				// ...Efforts
				$txt .= $GLOBALS['ds0'].'peffort=I'.$dat['aufwand_soll'];			// Planned Effort (in sec)
				$txt .= $GLOBALS['ds0'].'aeffort=I'.$dat['aufwand_ist'];			// Actual Effort (in sec)
				$txt .= $GLOBALS['ds0'].'meffort=I'.$dat['data']['max_zeit_aufw'];	// Max. Effort (in sec)
			}

			// ...Dates
			$vals=get_entry($dat['data']['soll_beginn'], 'datum');
			$vale=get_entry($dat['data']['soll_ende'], 'datum');
			if($vals == $vale)
				$val=$vals;
			else
				$val=$vals.' - '.$vale;
			$txt .= $GLOBALS['ds0'].'pdates=S'.$val;								// Planned Period of Time

			if($id > 0)
			{
				$val='';
				if(fxIsArray($GLOBALS['_CDP_arr']) && isset($GLOBALS['_CDP_arr'][$id]))
					$mma=array('minist'=>$GLOBALS['_CDP_arr'][$id]['asm'], 'maxist'=>$GLOBALS['_CDP_arr'][$id]['aem']);
				else
				{
					$sql="SELECT MIN(ist_beginn) AS minist, MAX(ist_beginn) AS maxist FROM zeitdaten WHERE projekt_id IN (SELECT projekt_id FROM projekte WHERE vorgangsnummer LIKE '".$dat['data']['vorgangsnummer']."%' AND mandanten_id=".$GLOBALS['fxpglobals']['client'].") AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.",".FXP_TE_TRAVELEXP.")";
					$mma=db_values($sql);
				}
				if(fxIsArray($mma))
				{
					$vals=get_entry($mma['minist'], 'datum');
					$vale=get_entry($mma['maxist'], 'datum');
					if($vals == $vale)
						$val=$vals;
					else
						$val=$vals.' - '.$vale;
				}
				$txt .= $GLOBALS['ds0'].'adates=S'.$val;							// Actual Period of Time
				$txt .= $GLOBALS['ds0'].'deadline=T'.$dat['data']['deadline'];		// Deadline

				// ...Responsible Persons
				$txt .= $GLOBALS['ds0'].'pm=I'.$dat['data']['projektmanager'];		// Project Manager
				$txt .= $GLOBALS['ds0'].'pl=I'.$dat['data']['projektleiter'];		// 1st Project Leader
				$txt .= $GLOBALS['ds0'].'dpl=I'.$dat['data']['vertreter'];			// 2nd Project Leader
				$txt .= $GLOBALS['ds0'].'ccp=I'.$dat['data']['ansprechpartner'];	// Customer Contact Person

				// ...Additional Responsible Persons
				if(!isset($GLOBALS['rrn']))
					$GLOBALS['rrn']=get_resp_role_names();
				if(fxIsArray($GLOBALS['rrn']))
				{
					foreach($GLOBALS['rrn'] as $rda)
					{
						$sql="SELECT parameter_int FROM projekte_parameter WHERE projekt_id=".$id." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND parameter_typ=".$rda['mafaehigkeit_id']." AND parameter_var='INT' AND parameter_txt='PV'";
						$txt .= $GLOBALS['ds0'].'pvr'.$rda['mafaehigkeit_id'].'=I'.(int)db_value($sql);
					}
				}

				// ...Budget
				$bcp=$dat['data']['abrechnungsart'];
				if(!$bcp || ($bcp == FXP_INV_MAT_HU))
					$bcp=FXP_INV_HOUR;
				$txt .= $GLOBALS['ds0'].'pbillcat=I'.$bcp;							// Billing Category (Persons)
				$bcm=$dat['data']['abrechnungsart_mat'];
				if(!$bcm || ($bcm == FXP_INV_HOUR))
					$bcm=FXP_INV_MAT_HU;
				$txt .= $GLOBALS['ds0'].'mbillcat=I'.$bcm;							// Billing Category (Material)
				$txt .= $GLOBALS['ds0'].'btype=I'.$dat['data']['budgettyp'];		// Budget Type
				foreach($bfields as $fname => $dfname)
					$txt .= $GLOBALS['ds0'].$fname.'=F'.$dat['data'][$dfname];		// Budget Fields

				// ...Info
				$txt .= $GLOBALS['ds0'].'pcat=I'.$dat['data']['kategorie_id'];		// Project Category
				$txt .= $GLOBALS['ds0'].'skill=T'.$psk;								// Skill Group / Skill
				$txt .= $GLOBALS['ds0'].'pmethod=I'.$dat['data']['methode'];		// Progress Determination Method

				$txt .= $GLOBALS['ds0'].'ref=T'.$dat['data']['ref_nummer'];			// Reference Number
				$txt .= $GLOBALS['ds0'].'loc=T'.$dat['data']['port'];				// Location
				$txt .= $GLOBALS['ds0'].'descr=T'.$dat['data']['beschreibung'];		// Description
				$txt .= $GLOBALS['ds0'].'note=T'.$dat['data']['anmerkung'];			// Annotation
				$txt .= $GLOBALS['ds0'].'inote=T'.$dat['data']['anmerkungintern'];	// Int. Annotation

				// ...Resource
				$res=prj_get_resources($id);
				$sor=sizeof($res);
				if($sor == 1)
				{
					$txt .= $GLOBALS['ds0'].'hr=I'.$res[0]['personen_id'];			// HR
					$txt .= $GLOBALS['ds0'].'pe=I'.$dat['aufwand_soll'];			// Planned resource effort (in sec)
					$txt .= $GLOBALS['ds0'].'pc=F1';								// Planned resource percentage
					$txt .= $GLOBALS['ds0'].'pp=F'.$res[0]['ek_projekt'];			// Purchase Price
					$txt .= $GLOBALS['ds0'].'sp=F'.$res[0]['vk_projekt'];			// Sell Price
				}
				else if($sor > 1)
				{
					$rids='';
					$rpes='';
					$rpcs='';
					$rpps='';
					$rsps='';
					foreach($res as $r)
					{
						$rids .= ','.$r['personen_id'];
						$rpes .= ','.$r['aufwand_std'];
						$rpcs .= ','.$r['prozent'];
						$rpps .= ','.$r['ek_projekt'];
						$rsps .= ','.$r['vk_projekt'];
					}
					$txt .= $GLOBALS['ds0'].'hr=T*'.substr($rids,1);				// HR
					$txt .= $GLOBALS['ds0'].'pe=T'.substr($rpes,1);					// Planned resource effort (in sec)
					$txt .= $GLOBALS['ds0'].'pc=T'.substr($rpcs,1);					// Planned resource percentage
					$txt .= $GLOBALS['ds0'].'pp=T'.substr($rpps,1);					// Purchase Price
					$txt .= $GLOBALS['ds0'].'sp=T'.substr($rsps,1);					// Sell Price
				}
				else
				{
					$txt .= $GLOBALS['ds0'].'hr=I0';								// HR
					$txt .= $GLOBALS['ds0'].'pe=I0';								// Planned resource effort (in sec)
					$txt .= $GLOBALS['ds0'].'pc=F0';								// Planned resource percentage
					$txt .= $GLOBALS['ds0'].'pp=F0';								// Purchase Price
					$txt .= $GLOBALS['ds0'].'sp=F0';								// Sell Price
				}

				// ...Userfields
				if(fxIsArray($GLOBALS['ufields']))
				{
					foreach($GLOBALS['ufields'] as $fid => $fdat)
					{
						$txt .= $GLOBALS['ds0'].$fdat['spezial'].'='.$fdat['ftype'];
						$uv='';
						if(fxIsArray($dat['userfield']))
							$uv=$dat['userfield'][$fid];
						if(!strlen($uv))
						{
							if($fdat['ftype'] == 'I')
								$uv=0;
							else if($fdat['ftype'] == 'F')
								$uv=0.0;
						}
						$txt .= $uv;
					}
				}
			}
		}

		$txt .= $GLOBALS['ds2'];

		// ...Dependency data=Projektnummer A | Projektnummer B | Typ | Minimumpuffer
		$dt='';
		if(fxIsArray($darr))
		{
			$pna=array();
			foreach($parr as $id => $dat)
				$pna[$id]=no_null($dat['data']['vorgangsnummer']);

			foreach($darr as $id => $dat)
			{
				if(isset($pna[$id]))
				{
					$pida=$pna[$id];
					if(isset($pna[$dat['projekt_b']]))
					{
						$pidb=$pna[$dat['projekt_b']];
						if(strlen($dt))
							$dt .= $GLOBALS['ds1'];

						$dt .= $pida;
						$dt .= $GLOBALS['ds0'].$pidb;
						$dt .= $GLOBALS['ds0'].$dat['abh_typ'];
						$dt .= $GLOBALS['ds0'].$dat['tagesanzahl'];
					}
				}
			}
		}
		$txt .= $dt;

		$txt .= $GLOBALS['ds2'];

		// ...Environment data 0=Start date, 1=End date, 2=Zoom, 3=ScrollTop, 4=ScrollLeft, 5=Structure width, 6=Detail width, 7=Gantt width, 8=HRP height, 9=Project line, 10=Prefix
		if(strlen($GLOBALS['env_param']))
			$ev=$GLOBALS['env_param'];
		else
		{
			$zoom=6;
			$ep='';
			$en='t189-env.txt';
			$efilename=fxSessionFile($en);
			if(file_exists($efilename))
			{
				$fp=@fopen($efilename,'r+');
				if($fp)
				{
					$ep=trim(fread($fp,filesize($efilename)));
					fclose($fp);
				}
			}
			if(strlen($ep))
			{
				$eva=explode('|',$ep);
				if(sizeof($eva) > 10)
				{
					$zoom=(int)$eva[2];
					$swidth=(int)$eva[5];
					$dwidth=(int)$eva[6];
					$gwidth=(int)$eva[7];
					$rpheight=(int)$eva[8];
				}
			}
			$ev='||'.$zoom.'|0|0|'.$swidth.'|'.$dwidth.'|'.$gwidth.'|'.$rpheight.'|-1|'.$prefix;
		}
		$txt .= $ev;
	}

	return $txt;
}

/**
 * ???FUNCTION???
 *
 * @param various $ps     - Mandatory parameter:                ???PARAMETER???
 * @param various $ds     - Mandatory parameter:                ???PARAMETER???
 * @param string  $prefix - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function matrixCSV2Array($ps,$ds,$prefix='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	matrixSetFileDividers();
	$dst=array($GLOBALS['ds0']=>'*0*',$GLOBALS['ds1']=>'*1*',$GLOBALS['ds2']=>'*2*',$GLOBALS['ds3']=>'*3*');

	if(strlen($prefix))
		$prefix=$prefix.' ';

	$psa=array('p'=>array(),'m'=>array(),'d'=>array());
	$nia=array();
	if(strlen($ps))
	{
		$pa=explode($GLOBALS['ds1'],$ps);
		if(fxIsArray($pa))
		{
			foreach($pa as $l => $pds)
			{
//echo('<b>'.$l.'</b><hr />'.fxHtmlEncode(strtr($pds,$dst)).'<br /><br />');
				$pda=explode($GLOBALS['ds0'],$pds);
				if(fxIsArray($pda))
				{
//fxDebug($pda, '$pda');
					// Milestone
					if($pda[0] == 'M')
					{
						$mc=sizeof($psa['m']);

						$psa['m'][$mc]=array(
							'id'		=> (int)$pda[1],
							'no'		=> $pda[2],
							'name'		=> $pda[3],
							'date'		=> $pda[4],
							'ddiff'		=> (int)$pda[7],
							'status'	=> (int)substr($pda[11],8),
							'folder'	=> $pda[8],
							'hidden'	=> (int)$pda[9]
						);
					}
					// Project
					else
					{
						$pc=sizeof($psa['p']);

						if($pda[0] == 'T')
							$type=FXP_PRJ_TASK;
						else if(!$pc)
							$type=FXP_PRJ_MAIN;
						else
							$type=FXP_PRJ_SUB;

						$psa['p'][$pc]=array(
							'type'		=> $type,
							'id'		=> (int)$pda[1],
							'no'		=> $pda[2],
							'name'		=> $prefix.$pda[3],
							'sdate'		=> $pda[4],
							'edate'		=> $pda[5],
							'folder'	=> $pda[8],
							'hidden'	=> (int)$pda[9],
							'info'		=> (int)$pda[10]
						);

						// Matrix columns
						for($c=11; $c<sizeof($pda); $c++)
						{
							$nve=explode('=',$pda[$c]);
							$vt=substr($nve[1],0,1);
							$vv=substr($nve[1],1);
							if(($vt == 'I') || ($vt == 'F'))
							{
								if((int)ceil((float)$vv) === (int)$vv)
									$vv=(int)$vv;
								else
									$vv=(float)$vv;
							}
							else if($vt == 'C')
							{
								if(strlen($vv) && $vv)
									$vv='1';
								else
									$vv='';
							}
							else
								$vv=(string)$vv;
							$psa['p'][$pc][$nve[0]]=$vv;
						}
						$nia[$psa['p'][$pc]['no']]=$psa['p'][$pc]['id'];
					}
				}
			}
		}
	}

	if(strlen($ds))
	{
		$da=explode($GLOBALS['ds1'],$ds);
		if(fxIsArray($da))
		{
			foreach($da as $l => $dds)
			{
//echo('<b>'.$l.'</b><hr />'.fxHtmlEncode(strtr($dds,$dst)).'<br /><br />');
				$dda=explode($GLOBALS['ds0'],$dds);
				if(fxIsArray($dda))
				{
//fxDebug($dda, '$dda');
					$dc=sizeof($psa['d']);

					$ida=0;
					if(isset($nia[$dda[0]]))
						$ida=$nia[$dda[0]];

					$idb=0;
					if(isset($nia[$dda[1]]))
						$idb=$nia[$dda[1]];

					$psa['d'][$dc]=array(
						'pida'		=> $ida,
						'pidb'		=> $idb,
						'pnoa'		=> $dda[0],
						'pnob'		=> $dda[1],
						'type'		=> $dda[2],
						'buffer'	=> (int)$dda[3]
					);
				}
			}
		}
	}

	return $psa;
}

/**
 * ???FUNCTION???
 *
 * @param various $pids            - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $with_dep_and_ms - Optional parameter (default = true):   ???PARAMETER???
 * @param boolean $with_history    - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function delProjectArray($pids,$with_dep_and_ms=true,$with_history=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mdate=db_value("SELECT MIN(ist_beginn) FROM zeitdaten WHERE projekt_id IN (".$pids.") AND ist_beginn IS NOT NULL AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.",".FXP_TE_TRAVELEXP.") AND mandanten_id=".$GLOBALS['fxpglobals']['client']);

	$pta=array(
		'abhaengigkeiten'		=> "DELETE FROM abhaengigkeiten WHERE (projekt_a IN (".$pids.") OR projekt_b IN (".$pids.")) AND mandanten_id=".$GLOBALS['fxpglobals']['client'],
		'artikelzuordnung'		=> "DELETE FROM artikelzuordnung WHERE projekt_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'],
		'auftrag'				=> "UPDATE auftrag SET projekt_id=NULL WHERE projekt_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'],
		'budget_detail'			=> "DELETE FROM budget_detail WHERE projekt_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'],
		'budget_summe'			=> "DELETE FROM budget_summe WHERE projekt_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'],
		'd_dok_verlauf'			=> "DELETE FROM d_dok_verlauf WHERE dok_id IN (SELECT dok_id FROM d_dokumente WHERE elter IN (".$pids.") AND elter_art=3 AND mandanten_id=".$GLOBALS['fxpglobals']['client'].") AND mandanten_id=".$GLOBALS['fxpglobals']['client'],
		'd_dok_transfer_log'	=> "DELETE FROM d_dok_transfer_log WHERE dok_id IN (SELECT dok_id FROM d_dokumente WHERE elter IN (".$pids.") AND elter_art=3 AND mandanten_id=".$GLOBALS['fxpglobals']['client'].") AND mandanten_id=".$GLOBALS['fxpglobals']['client'],
		'd_dokumente'			=> "DELETE FROM d_dokumente WHERE elter IN (".$pids.") AND elter_art=3 AND mandanten_id=".$GLOBALS['fxpglobals']['client'],
		'ekvk'					=> "DELETE FROM ekvk WHERE id IN (".$pids.") AND feldtyp=66 AND mandanten_id=".$GLOBALS['fxpglobals']['client'],
		'kostenzuordnung_pj'	=> "DELETE FROM kostenzuordnung WHERE id IN (".$pids.") AND kostenstellenart=904 AND mandanten_id=".$GLOBALS['fxpglobals']['client'],
		'kostenzuordnung_ze'	=> "DELETE FROM kostenzuordnung WHERE id IN (SELECT zeit_id FROM zeitdaten WHERE projekt_id IN (".$pids.") AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.",".FXP_TE_TRAVELEXP.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'].") AND kostenstellenart=".FXP_TE_TIMEREC." AND mandanten_id=".$GLOBALS['fxpglobals']['client'],
		'maeinsatz'				=> "DELETE FROM maeinsatz WHERE projekt_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'],
		'maplanung'				=> "DELETE FROM maplanung WHERE projekt_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'],
		'materialverbrauch'		=> "DELETE FROM materialverbrauch WHERE projekt_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'],
		'meilensteine'			=> "DELETE FROM meilensteine WHERE projekt_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'],
		'projekte_parameter'	=> "DELETE FROM projekte_parameter WHERE projekt_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'],
		'projektfortschritt'	=> "DELETE FROM projektfortschritt WHERE projekt_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'],
		'rechnung'				=> "DELETE FROM rechnung WHERE rechnungs_id IN (SELECT rechnungs_id FROM rechnungsposition WHERE projekt_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'].") AND mandanten_id=".$GLOBALS['fxpglobals']['client'],
		'rechnungseingang'		=> "DELETE FROM rechnungseingang WHERE projekt_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'],
		'rechnungsposition'		=> "DELETE FROM rechnungsposition WHERE projekt_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'],
		'rechte_referenz'		=> "DELETE FROM rechte_referenz WHERE id_referenz IN (".$pids.") AND rechte_ref=37 AND mandanten_id=".$GLOBALS['fxpglobals']['client'],
		'reisekosten'			=> "DELETE FROM reisekosten WHERE projekt_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'],
		'sachmittel'			=> "DELETE FROM sachmittel WHERE fuervorgang IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'],
		'taetigkeiten'			=> "DELETE FROM taetigkeiten WHERE zeit_id IN (SELECT zeit_id FROM zeitdaten WHERE projekt_id IN (".$pids.") AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.",".FXP_TE_TRAVELEXP.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'].") AND mandanten_id=".$GLOBALS['fxpglobals']['client'],
		'terminzuordnung'		=> "DELETE FROM terminzuordnung WHERE termin_id IN (SELECT termin_id FROM termine WHERE projekt_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'].") AND mandanten_id=".$GLOBALS['fxpglobals']['client'],
		'termine'				=> "DELETE FROM termine WHERE projekt_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'],
		'warnungen'				=> "DELETE FROM warnungen WHERE projekt_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'],
		'zeitdaten_freigabe'	=> "DELETE FROM zeitdaten_freigabe WHERE zeit_id IN (SELECT zeit_id FROM zeitdaten WHERE projekt_id IN (".$pids.") AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.",".FXP_TE_TRAVELEXP.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'].") AND mandanten_id=".$GLOBALS['fxpglobals']['client'],
		'zeitdaten_pj'			=> "DELETE FROM zeitdaten WHERE projekt_id IN (".$pids.") AND zeitart=102 AND mandanten_id=".$GLOBALS['fxpglobals']['client'],
		'zeitdaten_ze'			=> "DELETE FROM zeitdaten WHERE projekt_id IN (".$pids.") AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.",".FXP_TE_TRAVELEXP.") AND mandanten_id=".$GLOBALS['fxpglobals']['client']
	);

	if(!$with_dep_and_ms)
	{
		unset($pta['abhaengigkeiten']);
		unset($pta['meilensteine']);
	}

	// Bilance
	if(strlen($mdate))
		$pta['zeitdaten_bl']="DELETE FROM zeitdaten WHERE zeitart=104 AND ist_beginn>='".substr($mdate,0,6)."00000000' AND mandanten_id=".$GLOBALS['fxpglobals']['client'];

	// Userfields
	$sql="SELECT t.tabname FROM tabellen t, tab_felder f WHERE f.idintabellen<-9999 AND f.idinfelder=66 AND t.tabname LIKE '%_".$GLOBALS['fxpglobals']['client']."' AND f.idintabellen=t.id_tabellen";
	$utb=db_values($sql,'*');
//fxDebug($utb, $sql);
	if(fxIsArray($utb))
	{
		foreach($utb as $tb)
			$pta[strtolower($tb['tabname'])]="DELETE FROM ".strtolower($tb['tabname'])." WHERE projekt_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	}

	if($with_history)
	{
		if($with_dep_and_ms)
			$pta['abhaengigkeiten_history']	= "DELETE FROM abhaengigkeiten_history WHERE (projekt_a IN (".$pids.") OR projekt_b IN (".$pids.")) AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$pta['artikelzuordnung_history']	= "DELETE FROM artikelzuordnung_history WHERE projekt_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$pta['budget_detail_history']		= "DELETE FROM budget_detail_history WHERE projekt_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$pta['budget_summe_history']		= "DELETE FROM budget_summe_history WHERE projekt_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$pta['maeinsatz_history']			= "DELETE FROM maeinsatz_history WHERE projekt_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$pta['maplanung_history']			= "DELETE FROM maplanung_history WHERE projekt_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$pta['materialverbrauch_history']	= "DELETE FROM materialverbrauch_history WHERE projekt_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		if($with_dep_and_ms)
			$pta['meilensteine_history']	= "DELETE FROM meilensteine_history WHERE projekt_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$pta['projekte_parameter_history']	= "DELETE FROM projekte_parameter_history WHERE projekt_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$pta['rechte_referenz_history']		= "DELETE FROM rechte_referenz_history WHERE id_referenz IN (".$pids.") AND rechte_ref=37 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$pta['sachmittel_history']			= "DELETE FROM sachmittel_history WHERE fuervorgang IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$pta['termine_history']				= "DELETE FROM termine_history WHERE projekt_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$pta['zeitdaten_history']			= "DELETE FROM zeitdaten_history WHERE projekt_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];

		$pta['projekte_history']			= "DELETE FROM projekte_history WHERE (projekt_id IN (".$pids.") OR mutterprojekt_id IN (".$pids.")) AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	}

	// Project
	$pta['projekte']="DELETE FROM projekte WHERE (projekt_id IN (".$pids.") OR mutterprojekt_id IN (".$pids.")) AND mandanten_id=".$GLOBALS['fxpglobals']['client'];

	return $pta;
}

/**
 * ???FUNCTION???
 *
 * @param various $pids     - Mandatory parameter:                     ???PARAMETER???
 * @param array   $da       - Optional parameter (default = array()):  ???PARAMETER???
 * @param boolean $del      - Optional parameter (default = true):     ???PARAMETER???
 * @param boolean $pv_debug - Optional parameter (default = false):    ???PARAMETER???
 *
 * @return ???RETURN???
 */
function delProjectDocsArray($pids,$da=array(),$del=true,$pv_debug=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Get doc names
	if($pids !== false)
	{
		$sql="SELECT datei_name FROM d_dok_verlauf WHERE dok_id IN (SELECT dok_id FROM d_dokumente WHERE elter IN (".$pids.") AND elter_art=3 AND mandanten_id=".$GLOBALS['fxpglobals']['client'].") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$car=db_values($sql,'*');
//fxDebug($car,$sql);
		if(fxIsArray($car))
		{
			foreach($car as $c)
				$da[trim($c['datei_name'])]=true;
		}
	}

	if(!$del)
		return $da;

	if(!sizeof($da))
		return;

	$doc_path=$GLOBALS['fxpglobals']['settings']['dok_archiv'].$GLOBALS['fxpglobals']['client'];
	foreach($da as $dn => $dmy)
	{
		$filename =$doc_path.$GLOBALS['_oss'];
		$filename =preg_replace('#['.preg_quote($_oss).']+#', $GLOBALS['_oss'], $filename);
		$filename .= $dn;
		if($GLOBALS['_oss'] == '/')
			$ns='\\';
		else
			$ns='/';
		$filename=str_replace($ns, $GLOBALS['_oss'], $filename);
		if(@file_exists($filename))
		{
			if($pv_debug)
				echo('Delete file ['.$filename.']...<br />');
			else
			{
				@chmod($filename, 0777);
				unlink($filename);
			}
		}
	}
}

/**
 * ???FUNCTION???
 *
 * @param string $d - Optional parameter (default = ''): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getDTypes($d='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$dtypes=array(
		1=>array('FS'=>'(EA) Ende-zu-Anfang', 'FF'=>'(EE) Ende-zu-Ende', 'SS'=>'(AA) Anfang-zu-Anfang', 'SF'=>'(AE) Anfang-zu-Ende'),
		7=>array('FS'=>'(FS) Finish-to-Start', 'FF'=>'(FF) Finish-to-Finish', 'SS'=>'(SS) Start-to-Start', 'SF'=>'(SF) Start-to-Finish')
	);

	if(!strlen($d))
		return $dtypes;
	return $dtypes[$GLOBALS['fxpglobals']['lang']][$d];
}

/**
 * ???FUNCTION???
 *
 * @param various $pid - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function prjCheckOut($pid)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	//echo('<b>prjCheckOut</b>($pid='.$pid.')<br />');
	// Check in all checked out projects by this person
	@prjCheckIn();

	$pid=(int)$pid;
	$success=false;

	$sql="SELECT mutterprojekt_id FROM projekte WHERE projekt_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$mpi=(int)db_value($sql);
//fxDebug($mpi,$sql);
	if($mpi)
	{
		$sql  = "UPDATE projekte SET cout_id=".$GLOBALS['fxpglobals']['person'].", cout_date='".$GLOBALS['lts']."'";
		$sql .= " WHERE mutterprojekt_id=".$mpi." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];

		$success=db_query($sql);
//fxDebug($sql,'$success='.$success);

		// Write project log
		if($success)
			prjLog('write',$mpi,0);
	}

	return $success;
}

/**
 * ???FUNCTION???
 *
 * @param integer $pid - Optional parameter (default = 0): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function prjCheckIn($pid=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	//echo('<b>prjCheckIn</b>($pid='.$pid.')<br />');
	$pid=(int)$pid;
	$sql="";
	$success=true;

	if($pid > 0)
	{
		$s="SELECT mutterprojekt_id FROM projekte WHERE projekt_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$mpi=(int)db_value($s);
//fxDebug($mpi,$s);
		if($mpi)
		{
			$sql  = "UPDATE projekte SET cout_id=NULL, cout_date=NULL";
			$sql .= " WHERE mutterprojekt_id=".$mpi." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		}
	}
	else
	{
		$s ="SELECT mutterprojekt_id FROM projekte";
		$s .= " WHERE cout_id=".$GLOBALS['fxpglobals']['person']." AND cout_date LIKE '".$GLOBALS['lts']."' AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND projektart=".FXP_PRJ_MAIN;
		$tmp=db_values($s,'*');
//fxDebug($tmp,$s);
		if(fxIsArray($tmp))
		{
			$sql  = "UPDATE projekte SET cout_id=NULL, cout_date=NULL";
			$sql .= " WHERE cout_id=".$GLOBALS['fxpglobals']['person']." AND cout_date LIKE '".$GLOBALS['lts']."' AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$mpi='';
			foreach($tmp as $t)
			{
				if(strlen($mpi))
					$mpi .= ',';
				$mpi .= $t['mutterprojekt_id'];
			}
		}
	}

	if(strlen($sql))
	{
		$success=db_query($sql);
//fxDebug($sql,'$success='.$success.', $mpi='.$mpi);

		// Write project log
		if($success && strlen($mpi))
			prjLog('write',$mpi,99);
	}

	return $success;
}

/**
 * ???FUNCTION???
 *
 * @param boolean $all - Optional parameter (default = false): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function prjGetCheckedOut($all=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	//echo('<b>prjGetCheckedOut</b>($all='.$all.')<br />');
	if(isset($GLOBALS['prjCheckWarningCops']))
		return $GLOBALS['prjCheckWarningCops'];

	$warning=true;
	if(isset($GLOBALS['prjCheckWarning']))
		$warning=$GLOBALS['prjCheckWarning'];

	$cops='';

	$sql="SELECT mutterprojekt_id,vorgangsnummer,name_projekt,cout_id,cout_date FROM projekte WHERE cout_id IS NOT NULL AND cout_id>0 AND projektart=".FXP_PRJ_MAIN." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	if(!$all)
		$sql .= " AND (cout_id<>".$GLOBALS['fxpglobals']['person']." OR cout_date NOT LIKE '".$GLOBALS['lts']."')";
	$sql .= " ORDER BY vorgangsnummer";
	$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
	if(fxIsArray($tmp))
	{
		// Get all currently logged in persons of this client, that do really edit projects right now
		$u=array();
		$a=usrLog(false,$GLOBALS['fxpglobals']['client']);
//fxDebug($a,'$a');
		if(is_array($a) && sizeof($a))
		{
			$vpa=prjValidPF(true);
			foreach($a as $id => $d)
			{
				if(isset($vpa[$d['tr']]))
					$u[$d['person'].'-'.$d['lts']]=$d['tr'];
			}
		}
//fxDebug($u,'$u');

		$na=array();
		$ps='<br /><br /><ul>';
		foreach($tmp as $t)
		{
			$id=$t['cout_id'].'-'.$t['cout_date'];
			// Check in all projects that are not edited right now
			if(!isset($u[$id]))
			{
				$success=prjCheckIn($t['mutterprojekt_id']);
			}
			else
			{
				$cops .= ','.$t['mutterprojekt_id'];
				if($warning)
				{
					if(!isset($na[$id]))
					{
						$sql="SELECT pname,vorname FROM personen WHERE personen_id=".$t['cout_id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
						$psd=db_values($sql);
						if(fxIsArray($psd))
						{
							if($t['cout_id'] == $GLOBALS['fxpglobals']['person'])
								$na[$id]=substr($t['cout_date'],8).': '.$psd['pname'];
							else
								$na[$id]=$psd['pname'];
							if(strlen($psd['vorname']))
								$na[$id] .= ', '.$psd['vorname'];
						}
					}
					$ps .= '<li><b class=grey>'.no_null($t['vorgangsnummer']).'</b>&nbsp;&nbsp;'.$t['name_projekt'];
					if(isset($na[$id]))
						$ps .= '&nbsp;&nbsp;<i class=lightergrey>('.$na[$id].')</i>';
					$ps .= '</li>';
				}
			}
		}
		if(strlen($cops))
		{
			if($GLOBALS['fxpglobals']['tr'] == 189)	// Matrix
				$cops='';
			else
				$cops=substr($cops,1);

			if($warning)
			{
				$ps .= '</ul>';
				if($GLOBALS['fxpglobals']['tr'] == 189)	// Matrix
					$em=3024;	// The following main projects and/or their sub-projects can only be selected for display mode, because they are edited right now by another person:$var1
				else
					$em=3011;	// The following main projects and/or their sub-projects cannot be selected, because they are edited right now by another person:$var1
				error_msg($em,false,$ps);
			}
		}
	}
//echo('$cops='.$cops.'<br />');

	$GLOBALS['prjCheckWarningCops']=$cops;

	return $cops;
}

/**
 * ???FUNCTION???
 *
 * @param various $pid - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function prjIsCheckedOut($pid)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	//echo('<b>prjIsCheckedOut</b>($pid='.$pid.')<br />');
	$pid=(int)$pid;
	$chk=false;

	if($pid > 0)
	{
		$sql="SELECT mutterprojekt_id FROM projekte WHERE projekt_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$mpi=(int)db_value($sql);
//fxDebug($mpi,$sql);
		if($mpi)
		{
			$sql="SELECT cout_id,cout_date FROM projekte WHERE projekt_id=".$mpi." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$cop=db_values($sql);
//fxDebug($cop,$sql);
			$copi=(int)$cop['cout_id'];
			$copd=$cop['cout_date'];
			if(($copi && ($copi != $GLOBALS['fxpglobals']['person'])) || (strlen($copd) && ($copd != $GLOBALS['lts'])))
				$chk=true;
		}
	}
//fxDebug($chk,'prjIsCheckedOut('.$pid.')');
//echo('$chk='.$chk.'<br />');
	return $chk;
}

/**
 * ???FUNCTION???
 *
 * @param boolean $all - Optional parameter (default = false): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function prjValidPF($all=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pa=array(
		23	=> true,	// Detail: Textform mit Diagrammen
		189	=> true,	// Matrix: Graphisch als Gantt

		99	=> true,	// Kopieren

		43	=> true,	// Personen-Ressourcen (HR) zuordnen
		102	=> true,	// Material-Ressourcen (MR) zuordnen

		260	=> true,	// Budgetgenehmigung
	);

	if($all)
		return $pa;

	if(isset($pa[$GLOBALS['fxpglobals']['tr']]))
		return true;

	return false;
}

/**
 * ???FUNCTION???
 *
 * @return ???RETURN???
 */
function prjTableHasApproval()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Not yet supported
	return false;

	$v=$GLOBALS['fxpglobals']['dbparam']['versionscode_db'];
//echo('$v='.$v.'<hr />');
	// Column "freigabe" exists since V 5.2.0
	if(strlen($v) && ($v >= '005002000'))
		return true;

	if(db_check('freigabe', 'c', 'projekte'))
		return true;
	return false;
}

/**
 * ???FUNCTION???
 *
 * @param various $pid - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function prjGetLocation($pid)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$loc='';
	$pid=(int)$pid;
	$sql="SELECT vorgangsnummer FROM projekte WHERE projekt_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$pno=db_value($sql);
//fxDebug($pno,$sql);
	if(strlen($pno))
	{
		$pnc="vorgangsnummer LIKE '".$pno."'";
		while(strlen($pno))
		{
			$pno=substr($pno,0,-5);
			if(strlen($pno))
				$pnc .= " OR vorgangsnummer LIKE '".$pno."'";
		}
		$sql="SELECT vorgangsnummer,port FROM projekte WHERE (".$pnc.") AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY vorgangsnummer DESC";
		$pla=db_values($sql,'*');
//fxDebug($pla,$sql);
		if(fxIsArray($pla))
		{
			foreach($pla as $p)
			{
				if(strlen($p['port']))
				{
					$loc=$p['port'];
					break;
				}
			}
		}
	}

	return $loc;
}

/**
 * ???FUNCTION???
 *
 * @param various $t - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function poaTask($t)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!isset($GLOBALS['poaBR']))
		$GLOBALS['poaBR']='<br />';
	$tid=(int)$t['projekt_id'];
	$ta=array(
		'no'=>$t['vorgangsnummer'],'name'=>$t['name_projekt'],'ref'=>$t['ref_nummer'],'loc'=>$t['port'],
		'pstatus'=>$t['projekt_status'],'cat'=>$t['kategorie_id'],
		'peffort'=>round($t['aufwand_soll']/3600,4),'aeffort'=>round($t['aufwand_ist']/3600,4),
		'parent'=>""	//,'ppath'=>false
	);
	if(!strlen($ta['loc']))
		$ta['loc']=prjGetLocation($tid);

	if($GLOBALS['lvl'] > 0)
	{
		$vn=substr($t['vorgangsnummer'],0,-5);
		$pvn="vorgangsnummer LIKE '".$vn."'";
		while(strlen($vn))
		{
			$vn=substr($vn,0,-5);
			$pvn .= " OR vorgangsnummer LIKE '".$vn."'";
		}
		$sql="SELECT projekt_id,vorgangsnummer,name_projekt,kategorie_id FROM projekte WHERE (".$pvn.") AND mandanten_id=".$GLOBALS['cid']." ORDER BY vorgangsnummer DESC";
		$ppt=db_values($sql,'*');
//fxDebug($ppt,$sql);
		if(fxIsArray($ppt))
		{
			foreach($ppt as $lc => $ppd)
			{
				if($lc < $GLOBALS['lvl'])
					$ta['parent']=$ppd['name_projekt'].' >'.$GLOBALS['poaBR'].$ta['parent'];
			}
		}
	}

	return $ta;
}

/**
 * ???FUNCTION???
 *
 * @param various $p - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function prjAutoCorrect($p)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$c=$GLOBALS['fxpglobals']['client'];
	$p=(int)$p;
	$m=0;
	$n='';

	if($p > 0)
	{
		$s ="SELECT projekt_id,vorgangsnummer FROM projekte";
		$s .= " WHERE projekt_id IN (SELECT mutterprojekt_id FROM projekte WHERE projekt_id=".$p." AND mandanten_id=".$c.") AND mandanten_id=".$c;
		$t=db_values($s);
//fxDebug($t,$s);
		if(fxIsArray($t))
		{
			$m=$t['projekt_id'];
			$n=$t['vorgangsnummer'];
		}
	}
	if(($m <= 0) || !strlen($n))
		return 225;	// Error #$var1: Couldn´t get important data! Execution was canceled.

	// Check, if actual effort adjustment is necessary
	$s="SELECT zeitaufwand FROM zeitdaten WHERE mutterprojekt_id=".$m." AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.") AND zeitaufwand>0 AND mandanten_id=".$c;
	$o=(int)db_value($s);
//fxDebug($o,$s);
	if(!$o)
	{
		$s="SELECT MAX(aufwand_ist) AS me FROM projekte WHERE mutterprojekt_id=".$m." AND mandanten_id=".$c;
		$o=(int)db_value($s);
//fxDebug($o,$s);
	}
	if(!$o)
		return false;

	// Read project data for this main project
	$a=array();
	$s ="SELECT projekt_id,vorgangsnummer,aufwand_ist FROM projekte";
	$s .= " WHERE mutterprojekt_id=".$m." AND projektart<".FXP_PRJ_TASK." AND mandanten_id=".$c;
	$t=db_values($s,'*');
//fxDebug($t,$s);
	if(fxIsArray($t))
	{
		foreach($t as $d)
		{
			$a[$d['vorgangsnummer']]=array(
				'id'=>$d['projekt_id'],'deffort'=>(int)round($d['aufwand_ist'],0),'ceffort'=>0
			);
		}
	}
	else
		return 225;	// Error #$var1: Couldn´t get important data! Execution was canceled.
//fxDebug($a,'$a');

	// Read all efforts for this main project and recalculate
	$s ="SELECT z.projekt_id,z.zeitaufwand,p.vorgangsnummer FROM zeitdaten z, projekte p";
	$s .= " WHERE z.mutterprojekt_id=".$m." AND z.zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.") AND z.zeitaufwand>0 AND z.mandanten_id=".$c;
	$s .= " AND p.projekt_id=z.projekt_id AND p.mandanten_id=".$c;
	$t=db_values($s,'*');
//fxDebug($t,$s);
	if(fxIsArray($t))
	{
		foreach($t as $d)
		{
			$e=$d['vorgangsnummer'];
			while(strlen($e))
			{
				if(isset($a[$e]))
					$a[$e]['ceffort'] += $d['zeitaufwand'];
				$e=substr($e,0,-5);
			}
		}
	}
//fxDebug($a,'$a');

	// Check for errors and create correction sql
	$l=array();
	foreach($a as $d)
	{
		if($d['ceffort'] !== $d['deffort'])
			$l[$d['id']]="UPDATE projekte SET aufwand_ist=".$d['ceffort']." WHERE projekt_id=".$d['id']." AND mandanten_id=".$c;
	}

	// Execute correction sql's
	$r=false;
	if(sizeof($l))
	{
//fxDebug($l,'$l');
		foreach($l as $s)
		{
			$e=db_x($s);
//fxDebug($e,$s);
			if($e)
			{
				$r=1869;	// Error while updating the sum of the worked hours!
				break;
			}
		}
	}

	return $r;
}

/**
 * ???FUNCTION???
 *
 * @param array   $ea   - Optional parameter (default = array()): ???PARAMETER???
 * @param array   $ua   - Optional parameter (default = array()): ???PARAMETER???
 * @param boolean $send - Optional parameter (default = true):    ???PARAMETER???
 *
 * @return ???RETURN???
 */
function sendProjectMails($ea=array(), $ua=array(), $send=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	//echo('<b>sendProjectMails</b>();<br />');
	$sent=0;
	if(!isset($GLOBALS['_sent_success']))
		$GLOBALS['_sent_success']='';
	if(!isset($GLOBALS['_sent_error']))
		$GLOBALS['_sent_error']='';

	$hts=$GLOBALS['_history_timestamp'];
	if(strlen($hts) != 14)
	{
		if($send)
			return $sent;
		return $ea;
	}

	$htr=$GLOBALS['fxpglobals']['tr'];
	$cli=$GLOBALS['fxpglobals']['client'];

	// Get all changed project id's
	$sw=" WHERE zeitstempel='".$hts."' AND transid=".$htr." AND mandanten_id=".$cli;
	$ta=array(
		'projekte_history'		=> array('c'=>0, 's'=>""),
		'zeitdaten_history'		=> array('c'=>0, 's'=>" AND zeitart=102"),
		'budget_summe_history'	=> array('c'=>0, 's'=>""),
		'maplanung_history'		=> array('c'=>0, 's'=>" AND (sachmittel_id IS NULL OR sachmittel_id=0)")
	);
	$ia=array();
	foreach($ta as $table => $taa)
	{
		$sql="SELECT projekt_id,aktion,zeitstempel FROM ".$table.$sw.$taa['s'];
		$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
		if(fxIsArray($tmp))
		{
			$ta[$table]['c']=sizeof($tmp);
			foreach($tmp as $t)
			{
				$pid=(int)$t['projekt_id'];
				if(!isset($ia[$pid]))
					$ia[$pid]=array();
				if(!isset($ia[$pid][$table]))
					$ia[$pid][$table]=1;
				else
					$ia[$pid][$table]++;
			}
		}
	}
//fxDebug($ta,'$ta');
	if(!fxIsArray($ia))
	{
		if($send)
			return $sent;
		return $ea;
	}
//fxDebug($ia,'$ia');

	// Make csv string of all project id's
	$pc='';
	foreach($ia as $pid => $tables)
		$pc .= ','.$pid;
	$pc=substr($pc,1);
//fxDebug($pc,'$pc');

	// Get project infos
	$sw=" WHERE zeitstempel<='".$hts."' AND projekt_id IN (".$pc.") AND mandanten_id=".$cli;
	$so=" ORDER BY projekt_id, autoid DESC";
	$pa=array();
	// ...projects
	$sql  = "SELECT";
	$sql .= " autoid, zeitstempel, aktion, projekt_id, mutterprojekt_id, elter, vorgangsnummer, name_projekt, projektart, projekt_status, prioritaet, kategorie_id,";
	$sql .= " projektmanager, projektleiter, vertreter, kunde, ansprechpartner,";
	$sql .= " aufwand_soll, max_zeit_aufw, deadline, kategorie_id, beschreibung ";
	$sql .= "FROM projekte_history".$sw.$ta['projekte_history']['s'].$so;
	$tmp=db_values($sql,'*');
	if(fxIsArray($tmp))
	{
//fxDebug($tmp,$sql);
		foreach($tmp as $t)
		{
			$pid=(int)$t['projekt_id'];
			if(!isset($pa[$pid]))
				$pa[$pid]=array('pr'=>array('new'=>$t, 'old'=>array()), 'ti'=>array(), 'bu'=>array(), 're'=>array());
			else if(isset($ia[$pid]['projekte_history']) && $ia[$pid]['projekte_history'] && !sizeof($pa[$pid]['pr']['old']) && ($t['zeitstempel'] < $hts))
				$pa[$pid]['pr']['old']=$t;
		}
	}
	if(!fxIsArray($pa))
	{
		if($send)
			return $sent;
		return $ea;
	}
	// ...times
	$sql  = "SELECT";
	$sql .= " autoid, zeitstempel, aktion, projekt_id, soll_beginn, soll_ende, gen_beginn, gen_ende, ist_beginn, ist_ende ";
	$sql .= "FROM zeitdaten_history".$sw.$ta['zeitdaten_history']['s'].$so;
	$tmp=db_values($sql,'*');
	if(fxIsArray($tmp))
	{
//fxDebug($tmp,$sql);
		foreach($tmp as $t)
		{
			$pid=(int)$t['projekt_id'];
			if(isset($pa[$pid]))
			{
				if(!isset($pa[$pid]['ti']['new']))
					$pa[$pid]['ti']=array('new'=>$t, 'old'=>array());
				else if(isset($ia[$pid]['zeitdaten_history']) && $ia[$pid]['zeitdaten_history'] && !sizeof($pa[$pid]['ti']['old']) && ($t['zeitstempel'] < $hts))
					$pa[$pid]['ti']['old']=$t;
			}
		}
	}
	// ...budgets
	$sql  = "SELECT";
	$sql .= " autoid, zeitstempel, aktion, projekt_id, abrechnungsart, abrechnungsart_mat,";
	$sql .= " budgetrahmen_pers_int, budgetrahmen_mat_int, budgetrahmen_pers_ext, budgetrahmen_mat_ext,";
	$sql .= " budget_pers_int_gepl, budget_pers_ext_gepl, budget_mat_int_gepl, budget_mat_ext_gepl,";
	$sql .= " budget_pers_int_gen, budget_pers_ext_gen, budget_mat_int_gen, budget_mat_ext_gen,";
	$sql .= " budget_pers_int_akt, budget_pers_ext_akt, budget_mat_int_akt, budget_mat_ext_akt ";
	$sql .= "FROM budget_summe_history".$sw.$ta['budget_summe_history']['s'].$so;
	$tmp=db_values($sql,'*');
	if(fxIsArray($tmp))
	{
//fxDebug($tmp,$sql);
		foreach($tmp as $t)
		{
			$pid=(int)$t['projekt_id'];
			if(isset($pa[$pid]))
			{
				if(!isset($pa[$pid]['bu']['new']))
					$pa[$pid]['bu']=array('new'=>$t, 'old'=>array());
				else if(isset($ia[$pid]['budget_summe_history']) && $ia[$pid]['budget_summe_history'] && !sizeof($pa[$pid]['bu']['old']) && ($t['zeitstempel'] < $hts))
					$pa[$pid]['bu']['old']=$t;
			}
		}
	}
	// ...resources
	if($ta['maplanung_history']['c'])
	{
		$sql  = "SELECT";
		$sql .= " autoid, zeitstempel, aktion, projekt_id, personen_id, projekt_status, soll_beginn, soll_ende, auslastung, aufwand_std, prozent ";
		$sql .= "FROM maplanung_history".$sw.$ta['maplanung_history']['s'].$so;
		$tmp=db_values($sql,'*');
		if(fxIsArray($tmp))
		{
//fxDebug($tmp,$sql);
			foreach($tmp as $t)
			{
				$pid=(int)$t['projekt_id'];
				if(isset($pa[$pid]))
				{
					$pers=(int)$t['personen_id'];
					if(!isset($pa[$pid]['re'][$pers]))
					{
						if($t['zeitstempel'] >= $hts)
							$pa[$pid]['re'][$pers]=array('new'=>$t, 'old'=>array());
					}
					else if(isset($ia[$pid]['maplanung_history']) && ($ia[$pid]['maplanung_history'] > 1) && ($t['aktion'] != FXP_DELETE) && ($pa[$pid]['re'][$pers]['new']['aktion'] == FXP_CHANGE) && !sizeof($pa[$pid]['re'][$pers]['old']) && ($t['zeitstempel'] < $hts))
						$pa[$pid]['re'][$pers]['old']=$t;
				}
			}
		}
	}
//fxDebug($pa,'$pa');

	// Init variables
	$settings_backup=$GLOBALS['fxpglobals']['settings'];
	$lang_backup=$GLOBALS['fxpglobals']['lang'];

	$ma=array();
	$sa=array(1723,1724,1725,1729,1777,1778);

	// Get all setting modes for all persons
	$paa=array();
	$psa=array();
	foreach($pa as $pid => $pd)
	{
//echo('$pid=<b>'.$pid.'</b><hr />');
		$paa[$pid]=array();
		$paa[$pid]['pd']=$pd;

		$prn=$pa[$pid]['pr']['new'];
		$paa[$pid]['pnew']=$prn;
		if(fxIsArray($pa[$pid]['pr']['old']))
			$pro=$pa[$pid]['pr']['old'];
		else
			$pro=$prn;
		$paa[$pid]['pold']=$pro;
//fxDebug($prn,'$prn: '.$pid);
//fxDebug($pro,'$pro: '.$pid);

		$tin=$pa[$pid]['ti']['new'];
		$tin['soll_beginn']=substr($tin['soll_beginn'],0,8);
		$tin['soll_ende']=substr($tin['soll_ende'],0,8);
		$tin['ist_beginn']=substr($tin['ist_beginn'],0,8);
		$tin['ist_ende']=substr($tin['ist_ende'],0,8);
		$paa[$pid]['tnew']=$tin;
		if(fxIsArray($pa[$pid]['ti']['old']))
		{
			$tio=$pa[$pid]['ti']['old'];
			$tio['soll_beginn']=substr($tio['soll_beginn'],0,8);
			$tio['soll_ende']=substr($tio['soll_ende'],0,8);
			$tio['ist_beginn']=substr($tio['ist_beginn'],0,8);
			$tio['ist_ende']=substr($tio['ist_ende'],0,8);
		}
		else
			$tio=$tin;
		$paa[$pid]['told']=$tio;
//fxDebug($tin,'$tin: '.$pid);
//fxDebug($tio,'$tio: '.$pid);

		// Orders?
		$isorder=0;
		$isorder_sp=0;
		$isorder_mp=0;
		$sql="SELECT auftrags_id FROM auftrag WHERE projekt_id=".$prn['mutterprojekt_id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$isorder=(int)db_value($sql);
//fxDebug($isorder,$sql);
		if($isorder)
		{
			if($prn['projektart'] == FXP_PRJ_SUB)
				$isorder_sp=$isorder;
			if($prn['projektart'] == FXP_PRJ_MAIN)
				$isorder_mp=$isorder;
		}
		$paa[$pid]['isorder']=$isorder;
		$paa[$pid]['isorder_sp']=$isorder_sp;
		$paa[$pid]['isorder_mp']=$isorder_mp;

		$mpa=array();

		// Setting 1723: A project main responsible person (PM, P1 or P2) has been changed
		if(($prn['aktion'] == FXP_CHANGE) && (($prn['projektmanager'] != $pro['projektmanager']) || ($prn['projektleiter'] != $pro['projektleiter']) || ($prn['vertreter'] != $pro['vertreter'])))
			$mpa=getProjectMailPersons($mpa, 1723, $prn, $pro);

		// Setting 1725: A project date has been shifted
		if(($prn['aktion'] == FXP_CHANGE) && (($tin['soll_beginn'] != $tio['soll_beginn']) || ($tin['soll_ende'] != $tio['soll_ende'])))
			$mpa=getProjectMailPersons($mpa, 1725, $prn);

		// Setting 1724: The project status has been changed
		// Setting 1729: A project has been completed
		// Setting 1777: The project status has been changed for a project created from an order
		// Setting 1778: A main project created from an order has been completed
		if(($prn['aktion'] == FXP_CHANGE) && ($prn['projekt_status'] != $pro['projekt_status']))
		{
			if($prn['projekt_status'] == FXP_PS_COMPLETED)
			{
				$mpa=getProjectMailPersons($mpa, 1729, $prn);
				if($isorder_mp || $isorder_sp)
					$mpa=getProjectMailPersons($mpa, 1778, $prn);
			}
			else
			{
				$mpa=getProjectMailPersons($mpa, 1724, $prn);
				if($isorder_mp)
					$mpa=getProjectMailPersons($mpa, 1777, $prn);
			}
		}
//fxDebug($mpa,'$mpa');
		$paa[$pid]['mpa']=$mpa;

		if(sizeof($mpa))
		{
			foreach($mpa as $pers => $psettings)
			{
				// Person settings
				if(!isset($psa[$pers]))
				{
					foreach($sa as $set)
						$psa[$pers]['s'.$set]=false;
				}
				$paa[$pid]['hset']=array();
				foreach($sa as $set)
					$paa[$pid]['hset']['h'.$set]=false;
				$ea=explode(',', $psettings);
				foreach($ea as $set)
				{
					$psa[$pers]['s'.$set]=true;
					$paa[$pid]['hset']['h'.$set]=true;
				}
			}
		}
	}
//fxDebug($paa,'$paa');
//fxDebug($psa,'$psa');

	$pppc=array('projektmanager'=>false, 'projektleiter'=>false, 'vertreter'=>false, 'ansprechpartner'=>true);

	// Check for changes
	foreach($paa as $pid => $tpa)
	{
		$pd=$tpa['pd'];

		$prn=$tpa['pnew'];
		$pro=$tpa['pold'];
//fxDebug($prn,'$prn: '.$pid);
//fxDebug($pro,'$pro: '.$pid);

		$tin=$tpa['tnew'];
		$tio=$tpa['told'];
//fxDebug($tin,'$tin: '.$pid);
//fxDebug($tio,'$tio: '.$pid);

		// Orders?
		$isorder=$tpa['isorder'];
		$isorder_sp=$tpa['isorder_sp'];
		$isorder_mp=$tpa['isorder_mp'];

		$mpa=$tpa['mpa'];
		if(sizeof($mpa))
		{
			$md='email_prj_changed';	// Project Infomation^Project main responsible persons have been changed^Project dates have been shifted^Project status has been changed^Project has been finished
			$ms='email_prj_info';		// Project|AC~Status|AC~Priority^Project Manager|1st Project Leader|2nd Project Leader|Customer^AC~Planned Start|AC~Planned End^AC~Actual Start|AC~Actual End^Description
			$hc=colorCode('blue');

			foreach($mpa as $pers => $psettings)
			{
				// Get recipient data
				if(!isset($ua[$pers]))
				{
					$pdata=get_email_adress($pers,true,true);
					if(fxIsArray($pdata) && fxIsArray($pdata[$pers]))
						$ua[$pers]=$pdata[$pers];
					else
						$ua[$pers]=false;
//fxDebug($ua[$pers],'$ua['.$pers.']');
				}
				if(fxIsArray($ua[$pers]) && strlen($ua[$pers]['email']))
				{
					foreach($tpa['hset'] as $psv => $pset)
						${$psv}=$pset;
					foreach($psa[$pers] as $psv => $pset)
						${$psv}=$pset;

					$email=$ua[$pers]['email'];
					if(!isset($ea[$pers]))
						$ea[$pers]=array('email'=>$email);

					$GLOBALS['fxpglobals']['settings']=$ua[$pers]['settings'];
					$GLOBALS['fxpglobals']['lang']=$ua[$pers]['settings']['sprache'];
					$lang=$GLOBALS['fxpglobals']['lang'];

					if(!isset($ma[$md.'_'.$lang]))
						$ma[$md.'_'.$lang]=explode('^',get_message($md));
					if(!isset($ma[$ms.'_'.$lang]))
						$ma[$ms.'_'.$lang]=explode('^',get_message($ms));

					if(!isset($ea[$pers][$md]))
					{
						$ea[$pers][$md]=array('title'=>$ma[$md.'_'.$lang][0], 'headline'=>"", 'hset'=>array(), 'headers'=>$ma[$ms.'_'.$lang][0], 'data'=>array(), 'mbgcolor'=>$hc);

						// ...Headers
						if($s1723)
							$ea[$pers][$md]['headers'] .= '|'.$ma[$ms.'_'.$lang][1];
						if($s1724 || $s1725 || $s1729 || $s1777 || $s1778)
							$ea[$pers][$md]['headers'] .= '|'.$ma[$ms.'_'.$lang][2];
						if($s1729 || $s1778)
							$ea[$pers][$md]['headers'] .= '|'.$ma[$ms.'_'.$lang][3];
						$ea[$pers][$md]['headers'] .= '|'.$ma[$ms.'_'.$lang][4];
					}

					// ...Headlines
					if(($h1724 || $h1777) && !isset($ea[$pers][$md]['hset'][3]))
					{
						$ea[$pers][$md]['hset'][3]=true;
						if(strlen($ea[$pers][$md]['headline']))
							$ea[$pers][$md]['headline'] .= '; ';
						$ea[$pers][$md]['headline'] .= $ma[$md.'_'.$lang][3];
					}
					if(($h1729 || $h1778) && !isset($ea[$pers][$md]['hset'][4]))
					{
						$ea[$pers][$md]['hset'][4]=true;
						if(strlen($ea[$pers][$md]['headline']))
							$ea[$pers][$md]['headline'] .= '; ';
						$ea[$pers][$md]['headline'] .= $ma[$md.'_'.$lang][4];
					}
					if($h1723 && !isset($ea[$pers][$md]['hset'][1]))
					{
						$ea[$pers][$md]['hset'][1]=true;
						if(strlen($ea[$pers][$md]['headline']))
							$ea[$pers][$md]['headline'] .= '; ';
						$ea[$pers][$md]['headline'] .= $ma[$md.'_'.$lang][1];
					}
					if($h1725 && !isset($ea[$pers][$md]['hset'][2]))
					{
						$ea[$pers][$md]['hset'][2]=true;
						if(strlen($ea[$pers][$md]['headline']))
							$ea[$pers][$md]['headline'] .= '; ';
						$ea[$pers][$md]['headline'] .= $ma[$md.'_'.$lang][2];
					}

					// ...Project
					$data ='N~';
					if($isorder)
						$data .= '<font color="#'.colorCode('grey').'">(A'.$isorder.')</font>&nbsp;';
					$data .= '<b>'.no_null($prn['vorgangsnummer']).'</b>&nbsp;'.$prn['name_projekt'];
					// ...Status
					$data .= '|AC~N~';
					if($prn['projekt_status'] != $pro['projekt_status'])
						$data .= '<font style="color:#'.colorCode('grey').';">'.meldung($pro['projekt_status']).'</font>'.$GLOBALS['_carrow'].'<b>'.meldung($prn['projekt_status']).'</b>';
					else
						$data .= meldung($prn['projekt_status']);
					// ...Priority
					$data .= '|AC~N~';
					if($prn['prioritaet'] != $pro['prioritaet'])
						$data .= '<font style="color:#'.colorCode('grey').';">'.meldung($pro['prioritaet']).'</font>'.$GLOBALS['_carrow'].'<b>'.meldung($prn['prioritaet']).'</b>';
					else
						$data .= meldung($prn['prioritaet']);
					// ...Project Manager, 1st Project Leader, 2nd Project Leader + Customer
					if($s1723)
					{
						foreach($pppc as $ptype => $pparent)
						{
							$nameo=getPersonName($pro[$ptype], false, $pparent);
							if(!strlen($nameo) || ($nameo == '?'))
								$nameo='---';
							$namen=getPersonName($prn[$ptype], false, $pparent);
							if(!strlen($namen) || ($namen == '?'))
								$namen='---';
							$data .= '|';
							if($nameo != $namen)
								$data .= '<font style="color:#'.colorCode('grey').';">'.$nameo.'</font>'.$GLOBALS['_carrow'].'<b>'.$namen.'</b>';
							else
								$data .= $namen;
						}
					}
					if($s1724 || $s1725 || $s1729 || $s1777 || $s1778)
					{
						// ...Planned Start
						$data .= '|AC~N~';
						if($tin['soll_beginn'] != $tio['soll_beginn'])
							$data .= '<font style="color:#'.colorCode('grey').';">'.get_entry($tio['soll_beginn'],'datum').'</font>'.$GLOBALS['_carrow'].'<b>'.get_entry($tin['soll_beginn'],'datum').'</b>';
						else
							$data .= get_entry($tin['soll_beginn'],'datum');
						// ...Planned End
						$data .= '|AC~N~';
						if($tin['soll_ende'] != $tio['soll_ende'])
							$data .= '<font style="color:#'.colorCode('grey').';">'.get_entry($tio['soll_ende'],'datum').'</font>'.$GLOBALS['_carrow'].'<b>'.get_entry($tin['soll_ende'],'datum').'</b>';
						else
							$data .= get_entry($tin['soll_ende'],'datum');
					}
					if($s1729 || $s1778)
					{
						// ...Actual Start
						$data .= '|AC~N~';
						if($tin['ist_beginn'] != $tio['ist_beginn'])
							$data .= '<font style="color:#'.colorCode('grey').';">'.get_entry($tio['ist_beginn'],'datum').'</font>'.$GLOBALS['_carrow'].'<b>'.get_entry($tin['ist_beginn'],'datum').'</b>';
						else
							$data .= get_entry($tin['ist_beginn'],'datum');
						// ...Actual End
						$data .= '|AC~N~';
						if($tin['ist_ende'] != $tio['ist_ende'])
							$data .= '<font style="color:#'.colorCode('grey').';">'.get_entry($tio['ist_ende'],'datum').'</font>'.$GLOBALS['_carrow'].'<b>'.get_entry($tin['ist_ende'],'datum').'</b>';
						else
							$data .= get_entry($tin['ist_ende'],'datum');
					}
					// ...Description
					$data .= '|'.$prn['beschreibung'];

					$ea[$pers][$md]['data'][]=$data;
				}
			}
//fxDebug($ea,'$ea');
		}

		// Has an order been accepted? (Main project category changed from "neu" (=377) to "checked and accepted" (=403))
		if($isorder_mp && fxIsArray($pro) && ($pro['kategorie_id'] == 377) && ($prn['kategorie_id'] == 403))
		{
			$mpa=getProjectMailPersons(array(), 1732, $prn);
//fxDebug($mpa,'$mpa');
			if(sizeof($mpa))
			{
				$md='email_order_accepted';
				foreach($mpa as $pers => $dummy)
				{
					// Get recipient data
					if(!isset($ua[$pers]))
					{
						$pdata=get_email_adress($pers,true,true);
						if(fxIsArray($pdata) && fxIsArray($pdata[$pers]))
							$ua[$pers]=$pdata[$pers];
						else
							$ua[$pers]=false;
//fxDebug($ua[$pers],'$ua['.$pers.']');
					}
					$om=getOrderMessage($md, $pers, $prn, array(), false);
//fxDebug($om,'$om: '.$pers);
					if(isset($om['email']))
					{
						if(!isset($ea[$pers]))
							$ea[$pers]=array('email'=>$om['email']);
						if(!isset($ea[$pers][$md]))
							$ea[$pers][$md]=array('title'=>$om['title'], 'salut'=>$om['salut'], 'headline'=>$om['message'], 'headers'=>$om['headers'], 'data'=>$om['data'], 'mbgcolor'=>colorCode('green'));
						else
							$ea[$pers][$md]['data'][]=$om['data'][0];
					}
				}
			}
		}

		// Resource planning
		if(fxIsArray($pd['re']))
		{
//fxDebug($pd['re'],'re');

			$GLOBALS['fxpglobals']['settings']=$settings_backup;
			$GLOBALS['fxpglobals']['lang']=$lang_backup;

			// Setting 1727: Resource planning has been changed or Setting 1729: A project has been finished
			$rs1727=check_mail(1727,'MA');
			$rs1729=check_mail(1729,'MA');
			if($rs1727 || $rs1729)
			{
				foreach($pd['re'] as $pers => $rd)
				{
					if(fxIsArray($rd['new']))
					{
						$rn=$rd['new'];
						if(fxIsArray($rd['old']))
							$ro=$rd['old'];
						else
							$ro=$rn;

						if($rs1727 || ($rs1729 && ($rn['projekt_status'] == FXP_PS_COMPLETED) && ($rn['projekt_status'] != $ro['projekt_status'])))
						{
							// Get recipient data
							if(!isset($ua[$pers]))
							{
								$pdata=get_email_adress($pers,true,true);
								if(fxIsArray($pdata) && fxIsArray($pdata[$pers]))
									$ua[$pers]=$pdata[$pers];
								else
									$ua[$pers]=false;
//fxDebug($ua[$pers],'$ua['.$pers.']');
							}
							if(fxIsArray($ua[$pers]) && strlen($ua[$pers]['email']))
							{
								$email=$ua[$pers]['email'];
								if(!isset($ea[$pers]))
									$ea[$pers]=array('email'=>$email);

								$GLOBALS['fxpglobals']['settings']=$ua[$pers]['settings'];
								$GLOBALS['fxpglobals']['lang']=$ua[$pers]['settings']['sprache'];
								$lang=$GLOBALS['fxpglobals']['lang'];

								$md=-1;
								$ha=array(0=>'green', 2=>'blue', 4=>'red');
								if((($rn['aktion'] == FXP_CREATE) && ($rn['projekt_status'] == FXP_PS_ACTIVE)) || (($rn['aktion'] == FXP_CHANGE) && ($rn['projekt_status'] == FXP_PS_ACTIVE) && ($ro['projekt_status'] != FXP_PS_ACTIVE)))
									$md=0;
								else if(($rn['aktion'] == FXP_DELETE) && (($rn['projekt_status'] == FXP_PS_ACTIVE) || ($ro['projekt_status'] == FXP_PS_ACTIVE)))
									$md=4;
								else if(($rn['aktion'] == FXP_CHANGE) && (($rn['projekt_status'] == FXP_PS_ACTIVE) || ($ro['projekt_status'] == FXP_PS_ACTIVE)))
									$md=2;

								if($md >= 0)
								{
									// Resource Planning: New Task Assignment^The following tasks have been newly assigned to you^Resource Planning: Task Change^The following assigned tasks have changed^Resource Planning: Task Removal^The following tasks are no longer assigned to you
									$me='email_res_titles';
									if(!isset($ma[$me.'_'.$lang]))
										$ma[$me.'_'.$lang]=explode('^',get_message($me));

									// Task Name|AC~Status|AC~Priority|Description|AR~Percentage|AR~Planned Effort|AC~Planned Start|AC~Planned End
									$ms='email_res_tasks';
									if(!isset($ma[$ms.'_'.$lang]))
										$ma[$ms.'_'.$lang]=get_message($ms);

									$mp=$me.'_'.$md;

									if(!isset($ea[$pers][$mp]))
										$ea[$pers][$mp]=array('title'=>$ma[$me.'_'.$lang][$md], 'headline'=>$ma[$me.'_'.$lang][$md+1], 'headers'=>$ma[$ms.'_'.$lang], 'data'=>array(), 'mbgcolor'=>colorCode($ha[$md]));

									$data='N~<b>'.no_null($prn['vorgangsnummer']).'</b>&nbsp;'.$prn['name_projekt'];

									$data .= '|AC~N~';
									if(fxIsArray($ro) && ($rn['projekt_status'] != $ro['projekt_status']))
										$data .= '<font style="color:#'.colorCode('grey').';">'.meldung($ro['projekt_status']).'</font>'.$GLOBALS['_carrow'].'<b>'.meldung($rn['projekt_status']).'</b>';
									else
										$data .= meldung($rn['projekt_status']);

									$data .= '|AC~N~'.meldung($prn['prioritaet']);

									$data .= '|'.$prn['beschreibung'];

									$data .= '|AR~N~';
									if(fxIsArray($ro) && ($rn['prozent'] != $ro['prozent']))
										$data .= '<font style="color:#'.colorCode('grey').';">'.get_entry($ro['prozent']*100.0,'dezimal').'&nbsp;%</font>'.$GLOBALS['_carrow'].'<b>'.get_entry($rn['prozent']*100.0,'dezimal').'&nbsp;%</b>';
									else
										$data .= get_entry($rn['prozent']*100.0,'dezimal').'&nbsp;%';

									$data .= '|AR~N~';
									if(fxIsArray($ro) && ($rn['aufwand_std'] != $ro['aufwand_std']))
										$data .= '<font style="color:#'.colorCode('grey').';">'.get_entry($ro['aufwand_std']/3600,'dezimal').'&nbsp;h</font>'.$GLOBALS['_carrow'].'<b>'.get_entry($rn['aufwand_std']/3600,'dezimal').'&nbsp;h</b>';
									else
										$data .= get_entry($rn['aufwand_std']/3600,'dezimal').'&nbsp;h';

									$data .= '|AC~N~';
									if(fxIsArray($ro) && (substr($rn['soll_beginn'],0,8) != substr($ro['soll_beginn'],0,8)))
										$data .= '<font style="color:#'.colorCode('grey').';">'.get_entry($ro['soll_beginn'],'datum').'</font>'.$GLOBALS['_carrow'].'<b>'.get_entry($rn['soll_beginn'],'datum').'</b>';
									else
										$data .= get_entry($rn['soll_beginn'],'datum');

									$data .= '|AC~N~';
									if(fxIsArray($ro) && (substr($rn['soll_ende'],0,8) != substr($ro['soll_ende'],0,8)))
										$data .= '<font style="color:#'.colorCode('grey').';">'.get_entry($ro['soll_ende'],'datum').'</font>'.$GLOBALS['_carrow'].'<b>'.get_entry($rn['soll_ende'],'datum').'</b>';
									else
										$data .= get_entry($rn['soll_ende'],'datum');

									$ea[$pers][$mp]['data'][]=$data;
								}
							}
						}
					}
				}
			}

			$GLOBALS['fxpglobals']['settings']=$settings_backup;
			$GLOBALS['fxpglobals']['lang']=$lang_backup;

			$mpa=getProjectMailPersons(array(), 1727, $prn, array(), false);
//fxDebug($mpa,'$mpa');

			if(sizeof($mpa))
			{
				$mc=array(2=>'green', 'blue', 'red');
				foreach($mpa as $pers => $dummy)
				{
					// Get recipient data
					if(!isset($ua[$pers]))
					{
						$pdata=get_email_adress($pers,true,true);
						if(fxIsArray($pdata) && fxIsArray($pdata[$pers]))
							$ua[$pers]=$pdata[$pers];
						else
							$ua[$pers]=false;
//fxDebug($ua[$pers],'$ua['.$pers.']');
					}
					if(fxIsArray($ua[$pers]) && strlen($ua[$pers]['email']))
					{
						$email=$ua[$pers]['email'];
						if(!isset($ea[$pers]))
							$ea[$pers]=array('email'=>$email);

						$GLOBALS['fxpglobals']['settings']=$ua[$pers]['settings'];
						$GLOBALS['fxpglobals']['lang']=$ua[$pers]['settings']['sprache'];
						$lang=$GLOBALS['fxpglobals']['lang'];

						foreach($pd['re'] as $res => $rd)
						{
							if(fxIsArray($rd['new']))
							{
								$rn=$rd['new'];
								$ro=$rd['old'];

								$ms='email_res_planning';
								$md=0;
								if((($rn['aktion'] == FXP_CREATE) && ($rn['projekt_status'] == FXP_PS_ACTIVE)) || (($rn['aktion'] == FXP_CHANGE) && ($rn['projekt_status'] == FXP_PS_ACTIVE) && ($ro['projekt_status'] != FXP_PS_ACTIVE)))
									$md=2;	// has been assigned
								else if(($rn['aktion'] == FXP_DELETE) && (($rn['projekt_status'] == FXP_PS_ACTIVE) || ($ro['projekt_status'] == FXP_PS_ACTIVE)))
									$md=4;	// has been removed
								else if(($rn['aktion'] == FXP_CHANGE) && (($rn['projekt_status'] == FXP_PS_ACTIVE) || ($ro['projekt_status'] == FXP_PS_ACTIVE)))
									$md=3;	// has been changed

								if($md)
								{
									$me=$ms.'_'.$prn['projekt_id'];

									// Resource Planning^Resource planning of task^has been assigned^has been changed^has been removed^Person|AC~Action|AC~Task Status|AR~Percentage|AR~Planned Effort|AC~Planned Start|AC~Planned End
									if(!isset($ma[$ms.'_'.$lang]))
										$ma[$ms.'_'.$lang]=explode('^',get_message($ms));
									if(!isset($ea[$pers][$me]))
									{
										$ea[$pers][$me]=array('title'=>$ma[$ms.'_'.$lang][0], 'headers'=>$ma[$ms.'_'.$lang][5], 'data'=>array(), 'headline'=>$ma[$ms.'_'.$lang][1]);
										$ea[$pers][$me]['headline'] .= ': '.no_null($prn['vorgangsnummer']).' - '.$prn['name_projekt'];
									}

									$data='N~'.getPersonName($res);

									$data .= '|AC~N~<b style="color:#'.colorCode($mc[$md]).';">'.$ma[$ms.'_'.$lang][$md].'</b>';

									$data .= '|AC~N~';
									if(fxIsArray($ro) && ($rn['projekt_status'] != $ro['projekt_status']))
										$data .= '<font style="color:#'.colorCode('grey').';">'.meldung($ro['projekt_status']).'</font>'.$GLOBALS['_carrow'].'<b>'.meldung($rn['projekt_status']).'</b>';
									else
										$data .= meldung($rn['projekt_status']);

									$data .= '|AR~N~';
									if(fxIsArray($ro) && ($rn['prozent'] != $ro['prozent']))
										$data .= '<font style="color:#'.colorCode('grey').';">'.get_entry($ro['prozent']*100.0,'dezimal').'&nbsp;%</font>'.$GLOBALS['_carrow'].'<b>'.get_entry($rn['prozent']*100.0,'dezimal').'&nbsp;%</b>';
									else
										$data .= get_entry($rn['prozent']*100.0,'dezimal').'&nbsp;%';

									$data .= '|AR~N~';
									if(fxIsArray($ro) && ($rn['aufwand_std'] != $ro['aufwand_std']))
										$data .= '<font style="color:#'.colorCode('grey').';">'.get_entry($ro['aufwand_std']/3600,'dezimal').'&nbsp;h</font>'.$GLOBALS['_carrow'].'<b>'.get_entry($rn['aufwand_std']/3600,'dezimal').'&nbsp;h</b>';
									else
										$data .= get_entry($rn['aufwand_std']/3600,'dezimal').'&nbsp;h';

									$data .= '|AC~N~';
									if(fxIsArray($ro) && (substr($rn['soll_beginn'],0,8) != substr($ro['soll_beginn'],0,8)))
										$data .= '<font style="color:#'.colorCode('grey').';">'.get_entry($ro['soll_beginn'],'datum').'</font>'.$GLOBALS['_carrow'].'<b>'.get_entry($rn['soll_beginn'],'datum').'</b>';
									else
										$data .= get_entry($rn['soll_beginn'],'datum');

									$data .= '|AC~N~';
									if(fxIsArray($ro) && (substr($rn['soll_ende'],0,8) != substr($ro['soll_ende'],0,8)))
										$data .= '<font style="color:#'.colorCode('grey').';">'.get_entry($ro['soll_ende'],'datum').'</font>'.$GLOBALS['_carrow'].'<b>'.get_entry($rn['soll_ende'],'datum').'</b>';
									else
										$data .= get_entry($rn['soll_ende'],'datum');

									$ea[$pers][$me]['data'][]=$data;
								}
							}
						}
					}
				}
			}
		}
	}
//fxDebug($ea,'$ea');

	// Create and send emails
	if($send)
	{
		$sent=sendPersonMails($ea,$ua);

		$GLOBALS['fxpglobals']['settings']=$settings_backup;
		$GLOBALS['fxpglobals']['lang']=$lang_backup;

		return $sent;
	}

	$GLOBALS['fxpglobals']['settings']=$settings_backup;
	$GLOBALS['fxpglobals']['lang']=$lang_backup;

	return array('ea'=>$ea, 'ua'=>$ua);
}

/**
 * Create and send emails
 *
 * @param various $ea - Mandatory parameter: ???PARAMETER???
 * @param various $ua - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function sendPersonMails($ea, $ua)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sent=0;
	if(!isset($GLOBALS['_sent_success']))
		$GLOBALS['_sent_success']='';
	if(!isset($GLOBALS['_sent_error']))
		$GLOBALS['_sent_error']='';

	if(fxIsArray($ea))
	{
		foreach($ea as $pers => $ed)
		{
			if(fxIsArray($ed))
			{
				$email=$ed['email'];
				$title='';
				$message='';
				$content='';
				$GLOBALS['mail_attachments']=array();

				foreach($ed as $et => $eta)
				{
					if(($et != 'email') && fxIsArray($eta['data']))
					{
						// Title
						if(!strlen($title))
							$title=$eta['title'];
						else if(substr($title,-1) != '.')
							$title .= ' ...';

						// Content
						if(!strlen($content))
						{
							// ...Info
							if(isset($eta['info']) && strlen($eta['info']))
								$content .= $eta['info'].'<br /><br /><br />'.$GLOBALS['nl'];

							// ...Salutation
							if(isset($eta['salut']) && strlen($eta['salut']))
								$content .= $eta['salut'].'<br /><br /><br />'.$GLOBALS['nl'];
							else if(!isset($eta['salut']))
							{
								$GLOBALS['fxpglobals']['lang']=$ua[$pers]['settings']['sprache'];
								$lang=$GLOBALS['fxpglobals']['lang'];

								$s=$ua[$pers]['anrede2'];
								if(!isset($ma[$s.'_'.$lang]))
									$ma[$s.'_'.$lang]=meldung($s);

								$content .= str_replace('$var1', $ua[$pers]['name2'], $ma[$s.'_'.$lang]).'<br /><br /><br />'.$GLOBALS['nl'];
							}

							// ...Message
							if(isset($eta['message']) && strlen($eta['message']))
								$content .= $eta['message'].'<br /><br /><br />'.$GLOBALS['nl'];
						}
						else
							$content .= '<br /><br />'.$GLOBALS['nl'];

						if(isset($eta['headers']))
							$content .= mailDataTable($eta['headers'], $eta['data'], $eta['headline'], $eta['mbgcolor']);
						else
						{
							foreach($eta['data'] as $dlc => $dlcontent)
								$content .= $dlcontent.'<br /><br />'.$GLOBALS['nl'];
						}

						// iCalender attachment
						if(isset($eta['ics']) && ($eta['ics'] > 0))
							$ics_filename=iCalendar($eta['ics']);
					}
				}

				// Send email
				if(strlen($title) && strlen($content))
				{
//echo('<div style="border:1px solid #444444;border-top-left-radius:8px;border-top-right-radius:8px;padding:8px;color:#ffffff;background:#006b9f;"><b>'.$title.'</b> <i>(recipient='.$pers.')</i></div><div style="border:1px solid #444444;border-top:0;border-bottom-left-radius:8px;border-bottom-right-radius:8px;padding:8px;background:#ffffff;margin-bottom:6px;">'.$content.'</div>'.$GLOBALS['nl']); fxDebug($GLOBALS['mail_attachments'], 'mail_attachments');
//$send_error=true;
					$send_error=send_mail($pers, $title, $content, 0, 0, $GLOBALS['mail_attachments']);
					if($send_error)
					{
						$sent=false;
						$GLOBALS['_sent_error'] .= '<br />&middot; <b class="red">'.getPersonName($pers,false).'</b>';
					}
					else
					{
						if($sent !== false)
							$sent++;
						$GLOBALS['_sent_success'] .= '<br />&middot; <b class="grey">'.getPersonName($pers,false).'</b>';
					}
				}

				// Delete attachments?
				$deleted=delAttachments();
			}
		}
	}

	return $sent;
}

/**
 * ???FUNCTION???
 *
 * @param various $headers  - Mandatory parameter:                   ???PARAMETER???
 * @param various $data     - Mandatory parameter:                   ???PARAMETER???
 * @param string  $headline - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $hbgcolor - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $width    - Optional parameter (default = '80%'):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function mailDataTable($headers, $data, $headline='', $hbgcolor='', $width='80%')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!isset($GLOBALS['class_fontstyle']))
		$GLOBALS['class_fontstyle']='font-family:font-family:verdana,arial,helvetica,sans-serif;font-size:12pt;';

	$mt='<table width="'.$width.'" border="0" cellpadding="2" cellspacing="1" bgcolor="#cccccc" style="border-radius:4px;">'.$GLOBALS['nl'];

	// Headline
	if(strlen($headline))
	{
		if(!strlen($hbgcolor))
			$hbgcolor='787878';

		$mt .= '	<tr>'.$GLOBALS['nl'];
		$mt .= '		<td width="100%" bgcolor="#'.$hbgcolor.'" style="border-top-left-radius:4px;border-top-right-radius:4px;border-bottom:1px solid #222222;"><b style="color:#ffffff;'.$GLOBALS['class_fontstyle'].'">'.$headline.'</b></td>'.$GLOBALS['nl'];
		$mt .= '	</tr>'.$GLOBALS['nl'];
	}

	$mt .= '	<tr>'.$GLOBALS['nl'];
	$mt .= '		<td>'.$GLOBALS['nl'];

	$mt .= '			<table width="100%" border="0" cellpadding="2" cellspacing="1" bgcolor="#bbbbbb">'.$GLOBALS['nl'];

	// Headers
	$ha=explode('|', $headers);
	$mt .= '				<tr>'.$GLOBALS['nl'];
	foreach($ha as $ht)
	{
		$at=tdInfo($ht);
		$mt .= '					<td nowrap bgcolor="#aaaaaa"'.$at['info'].'>&nbsp;<b>'.$at['text'].'</b>&nbsp;</td>'.$GLOBALS['nl'];
	}
	$mt .= '				</tr>'.$GLOBALS['nl'];
	
	// Data
	$c=0;
	$ca=array(array('#dddddd','#f8f8f8'), array('#d8d8d8','#f1f1f1'));
	foreach($data as $dlc => $dla)
	{
		$da=explode('|', $dla);
		$mt .= '				<tr>'.$GLOBALS['nl'];
		foreach($da as $dc => $dt)
		{
			if($dc)
				$tc=$ca[$c%2][1];
			else
				$tc=$ca[$c%2][0];

			$at=tdInfo($dt);
			$mt .= '					<td bgcolor="'.$tc.'" valign="top"'.$at['info'].'>&nbsp;'.$at['text'].'&nbsp;</td>'.$GLOBALS['nl'];
		}
		$mt .= '				</tr>'.$GLOBALS['nl'];
		$c++;
	}

	$mt .= '			</table>'.$GLOBALS['nl'];

	$mt .= '		</td>'.$GLOBALS['nl'];
	$mt .= '	</tr>'.$GLOBALS['nl'];
	$mt .= '</table>'.$GLOBALS['nl'];

	return $mt;
}

/**
 * ???FUNCTION???
 *
 * @param various $t - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function tdInfo($t)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$info='';
	$text=$t;

	if(strlen($t))
	{
		$ta=explode('~',$t);
		$s=sizeof($ta);
		$text=$ta[$s-1];
		if($s > 1)
		{
			for($i=0; $i<$s-1; $i++)
			{
				$c=strtoupper(substr($ta[$i],0,1));
				$v=substr($ta[$i],1);
				if($c == 'A')
				{
					$u=strtoupper(substr($v,0,1));
					$info .= ' align="';
					if($u == 'C')
						$info .= 'center';
					else if($u == 'R')
						$info .= 'right';
					else
						$info .= 'left';
					$info .= '"';
				}
				else if($c == 'C')
					$info .= ' colspan="'.$v.'"';
				else if($c == 'R')
					$info .= ' rowspan="'.$v.'"';
				else if($c == 'W')
					$info .= ' width="'.$v.'"';
				else if($c == 'H')
					$info .= ' height="'.$v.'"';
				else if($c == 'B')
					$info .= ' bgcolor="'.$v.'"';
				else if($c == 'N')
					$info .= ' nowrap';
			}
		}
	}

	if(!strlen($text))
		$text='<i style="color:#'.colorCode('grey').';">---</i>';
	else if(strpos($text,'&#8594;') !== false)
	{
		$text=str_replace('"></', '">---</', $text);
		if(substr($text,0,9) == '&nbsp;<br')
			$text='<i style="color:#'.colorCode('grey').';">---</i>'.$text;
		if(substr($text,-9) == 'nt>&nbsp;')
			$text .= '<i style="color:#'.colorCode('grey').';">---</i>';
	}

	return array('info'=>$info, 'text'=>$text);
}

/**
 * ???FUNCTION???
 *
 * @param various $pid - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getProjectName($pid)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pid=(int)$pid;
	if($pid <= 0)
		return meldung(681);	// Unbekannt

	$sql="SELECT vorgangsnummer,name_projekt FROM projekte WHERE projekt_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$pvn=db_values($sql);
//fxDebug($pvn,$sql);
	if(fxIsArray($pvn))
		$vn=no_null($pvn['vorgangsnummer']).' - '.$pvn['name_projekt'];
	else
		$vn='['.$pid.']';

	return $vn;
}

/**
 * ???FUNCTION???
 *
 * @param various $pid   - Mandatory parameter:                  ???PARAMETER???
 * @param boolean $human - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getAssignedResources($pid, $human=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$hra=array();
	if($human)
		$sql="SELECT personen_id AS id, aufwand_std FROM maplanung WHERE projekt_id=".(int)$pid." AND (sachmittel_id=0 OR sachmittel_id IS NULL) AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	else
		$sql="SELECT sachmittel_id AS id, aufwand_std FROM maplanung WHERE projekt_id=".(int)$pid." AND (personen_id=0 OR personen_id IS NULL) AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
	if(fxIsArray($tmp))
	{
		foreach($tmp as $t)
			$hra[$t['id']]=$t['aufwand_std'];
	}

	return $hra;
}

/**
 * ???FUNCTION???
 *
 * @param various $sec  - Mandatory parameter:                ???PARAMETER???
 * @param integer $unit - Optional parameter (default = 0):   ???PARAMETER???
 * @param string  $ret  - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function effortSec2Unit($sec, $unit=0, $ret='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sec=(int)$sec;

	if(!$unit)
	{
		$unit=(int)$GLOBALS['fxpglobals']['settings']['munit'];
		if(!$unit)
			$unit=(int)$GLOBALS['fxpglobals']['punit'];
		if(!$unit)
			$unit=484;	// Hrs.
	}

	if($unit == 856)		// Min.
		$calc=60;
	else if($unit == 485)	// PD
		$calc=$GLOBALS['fxpglobals']['settings']['mtag']*3600;
	else if($unit == 923)	// PW
		$calc=$GLOBALS['fxpglobals']['settings']['mwoche']*$GLOBALS['fxpglobals']['settings']['mtag']*3600;
	else if($unit == 486)	// PM
		$calc=$GLOBALS['fxpglobals']['settings']['mmonat']*$GLOBALS['fxpglobals']['settings']['mtag']*3600;
	else if($unit == 487)	// PY
		$calc=$GLOBALS['fxpglobals']['settings']['mjahr']*$GLOBALS['fxpglobals']['settings']['mtag']*3600;
	else					// Std.
		$calc=3600;

	$ua=array('sec'=>$sec, 'calc'=>$calc, 'unit1'=>$unit, 'unit2'=>meldung($unit), 'val1'=>0.0, 'val2'=>"");

	if($sec != 0)
		$ua['val1']=(float)($sec/$calc);
	$ua['val2']=get_entry($ua['val1'],'dezimal');
//fxDebug($ua, '$ua');

	if(strlen($ret))
		return $ua[$ret];
	return $ua;
}

/**
 * ???FUNCTION???
 *
 * @param various $val  - Mandatory parameter:                ???PARAMETER???
 * @param integer $unit - Optional parameter (default = 0):   ???PARAMETER???
 * @param string  $ret  - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function effortUnit2Sec($val, $unit=0, $ret='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$val=get_double($val);

	if(!$unit)
	{
		$unit=(int)$GLOBALS['fxpglobals']['settings']['munit'];
		if(!$unit)
			$unit=(int)$GLOBALS['fxpglobals']['punit'];
		if(!$unit)
			$unit=484;	// Hrs.
	}

	if($unit == 856)		// Min.
		$calc=60;
	else if($unit == 485)	// PD
		$calc=$GLOBALS['fxpglobals']['settings']['mtag']*3600;
	else if($unit == 923)	// PW
		$calc=$GLOBALS['fxpglobals']['settings']['mwoche']*$GLOBALS['fxpglobals']['settings']['mtag']*3600;
	else if($unit == 486)	// PM
		$calc=$GLOBALS['fxpglobals']['settings']['mmonat']*$GLOBALS['fxpglobals']['settings']['mtag']*3600;
	else if($unit == 487)	// PY
		$calc=$GLOBALS['fxpglobals']['settings']['mjahr']*$GLOBALS['fxpglobals']['settings']['mtag']*3600;
	else					// Std.
		$calc=3600;

	$ua=array('sec'=>0, 'calc'=>$calc, 'unit1'=>$unit, 'unit2'=>meldung($unit), 'val1'=>$val, 'val2'=>get_entry($val,'dezimal'));

	if($val != 0.0)
		$ua['sec']=(int)($val*$calc);
//fxDebug($ua, '$ua');

	if(strlen($ret))
		return $ua[$ret];
	return $ua;
}

/**
 * ???FUNCTION???
 *
 * @param various $md           - Mandatory parameter:                     ???PARAMETER???
 * @param various $pers         - Mandatory parameter:                     ???PARAMETER???
 * @param various $pnew         - Mandatory parameter:                     ???PARAMETER???
 * @param array   $pold         - Optional parameter (default = array()):  ???PARAMETER???
 * @param boolean $with_content - Optional parameter (default = true):     ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getOrderMessage($md, $pers, $pnew, $pold=array(), $with_content=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Init variables
	$settings_backup=$GLOBALS['fxpglobals']['settings'];
	$lang_backup=$GLOBALS['fxpglobals']['lang'];

	if(!isset($GLOBALS['_ua']))
		$GLOBALS['_ua']=array();
	if(!isset($GLOBALS['_ma']))
		$GLOBALS['_ma']=array();
	$ea=array();

//fxDebug($pnew,'$pnew');
//fxDebug($pold,'$pold');
	if(fxIsArray($pnew))
	{
		// Get recipient data
		if(!isset($GLOBALS['_ua'][$pers]))
		{
			$pdata=get_email_adress($pers,true,true);
			if(fxIsArray($pdata) && fxIsArray($pdata[$pers]))
				$GLOBALS['_ua'][$pers]=$pdata[$pers];
			else
				$GLOBALS['_ua'][$pers]=false;
//fxDebug($GLOBALS['_ua'][$pers],'$ua['.$pers.']');
		}

		if(fxIsArray($GLOBALS['_ua'][$pers]) && strlen($GLOBALS['_ua'][$pers]['email']))
		{
			$ea['email']=$GLOBALS['_ua'][$pers]['email'];

			$GLOBALS['fxpglobals']['settings']=$GLOBALS['_ua'][$pers]['settings'];
			$GLOBALS['fxpglobals']['lang']=$GLOBALS['_ua'][$pers]['settings']['sprache'];
			$lang=$GLOBALS['fxpglobals']['lang'];

			// ...Salutation
			$s=$GLOBALS['_ua'][$pers]['anrede2'];
			if(!isset($GLOBALS['_ma'][$s.'_'.$lang]))
				$GLOBALS['_ma'][$s.'_'.$lang]=meldung($s);

			$salut=str_replace('$var1', $GLOBALS['_ua'][$pers]['name2'], $GLOBALS['_ma'][$s.'_'.$lang]);

			// ...Project Number
			$data='N~';
			if(sizeof($pold) && ($pnew['vorgangsnummer'] !== $pold['vorgangsnummer']))
				$data .= '<font style="color:#'.colorCode('grey').';">'.no_null($pold['vorgangsnummer']).'</font>'.$GLOBALS['_carrow'];
			$data .= '<b style="color:#'.colorCode('green').';">'.no_null($pnew['vorgangsnummer']).'</b>';
			// ...Project Name
			$data .= '|N~';
			if(sizeof($pold) && ($pnew['name_projekt'] !== $pold['name_projekt']))
				$data .= '<font style="color:#'.colorCode('grey').';">'.$pold['name_projekt'].'</font>'.$GLOBALS['_carrow'].'<b>'.$pnew['name_projekt'].'</b>';
			else
				$data .= $pnew['name_projekt'];
			// ...Customer
			$data .= '|N~';
			if(sizeof($pold) && ($pnew['ansprechpartner'] !== $pold['ansprechpartner']))
				$data .= '<font style="color:#'.colorCode('grey').';">'.getPersonName($pold['ansprechpartner'],false,true).'</font>'.$GLOBALS['_carrow'].'<b>'.getPersonName($pnew['ansprechpartner'],false,true).'</b>';
			else
				$data .= getPersonName($pnew['ansprechpartner'],false,true);
			if($md == 'email_order_accepted')
			{
				// ...Project Manager
				$data .= '|N~';
				if(sizeof($pold) && ($pnew['projektmanager'] !== $pold['projektmanager']))
					$data .= '<font style="color:#'.colorCode('grey').';">'.getPersonName($pold['projektmanager'],false).'</font>'.$GLOBALS['_carrow'].'<b>'.getPersonName($pnew['projektmanager'],false).'</b>';
				else
					$data .= getPersonName($pnew['projektmanager'],false);
				// ...1st Project Leader
				$data .= '|N~';
				if(sizeof($pold) && ($pnew['projektleiter'] !== $pold['projektleiter']))
					$data .= '<font style="color:#'.colorCode('grey').';">'.getPersonName($pold['projektleiter'],false).'</font>'.$GLOBALS['_carrow'].'<b>'.getPersonName($pnew['projektleiter'],false).'</b>';
				else
					$data .= getPersonName($pnew['projektleiter'],false);
				// ...2nd Project Leader
				$data .= '|N~';
				if(sizeof($pold) && ($pnew['vertreter'] !== $pold['vertreter']))
					$data .= '<font style="color:#'.colorCode('grey').';">'.getPersonName($pold['vertreter'],false).'</font>'.$GLOBALS['_carrow'].'<b>'.getPersonName($pnew['vertreter'],false).'</b>';
				else
					$data .= getPersonName($pnew['vertreter'],false);
			}
			// ...Planned Start
			$data .= '|AC~N~';
			if(sizeof($pold) && ($pnew['soll_beginn'] !== $pold['soll_beginn']))
				$data .= '<font style="color:#'.colorCode('grey').';">'.get_entry($pold['soll_beginn'],'datum').'</font>'.$GLOBALS['_carrow'].'<b>'.get_entry($pnew['soll_beginn'],'datum').'</b>';
			else
				$data .= get_entry($pnew['soll_beginn'],'datum');
			// ...Planned End
			$data .= '|AC~N~';
			if(sizeof($pold) && ($pnew['soll_ende'] !== $pold['soll_ende']))
				$data .= '<font style="color:#'.colorCode('grey').';">'.get_entry($pold['soll_ende'],'datum').'</font>'.$GLOBALS['_carrow'].'<b>'.get_entry($pnew['soll_ende'],'datum').'</b>';
			else
				$data .= get_entry($pnew['soll_ende'],'datum');
			// ...Description
			$nl_repl=array("<br \>\r\n"=>"<br \>\r\n", "<br />\r\n"=>"<br \>\r\n", "\r\n"=>"<br \>\r\n", "\n"=>"<br \>\n"); 
			$data .= '|W50%~N~'.strtr($pnew['beschreibung'],$nl_repl);

			// 126=email_order_newproject:	New main project^the following main project has just been created from an order^Project Number|Project Name|Customer|AC~Planned Start|AC~Planned End|Description
			// 129=email_order_accepted:	Order accepted^the order of the following main project has been checked and accepted^Project Number|Project Name|Customer|Project Manager|1st Project Leader|2nd Project Leader|AC~Planned Start|AC~Planned End|Description
			// 131=email_order_changed:		Order changed^the order of the following main project has been changed^Project Number|Project Name|Customer|AC~Planned Start|AC~Planned End|Description
			// 134=email_order_deleted:		Order deleted^the order of the following main project has been deleted^Project Number|Project Name|Customer|AC~Planned Start|AC~Planned End|Description
			if(!isset($GLOBALS['_ma'][$md.'_'.$lang]))
				$GLOBALS['_ma'][$md.'_'.$lang]=explode('^',get_message($md));

			$ea['title']	= $GLOBALS['_ma'][$md.'_'.$lang][0];
			$ea['salut']	= $salut;
			$ea['message']	= $GLOBALS['_ma'][$md.'_'.$lang][1];
			$ea['headers']	= $GLOBALS['_ma'][$md.'_'.$lang][2];
			$ea['data']		= array($data);

			if($with_content)
			{
				$ea['content'] =$ea['salut'].'<br /><br /><br />'.$GLOBALS['nl'];
				$ea['content'] .= $ea['message'].'<br /><br /><br />'.$GLOBALS['nl'];
				$ea['content'] .= mailDataTable($ea['headers'], $ea['data']);
//echo('<div style="border:1px solid #444444;border-top-left-radius:8px;border-top-right-radius:8px;padding:8px;color:#ffffff;background:#006b9f;"><b>'.$ea['title'].'</b> <i>(recipient='.$pers.')</i></div><div style="border:1px solid #444444;border-top:0;border-bottom-left-radius:8px;border-bottom-right-radius:8px;padding:8px;background:#ffffff;margin-bottom:6px;">'.$ea['content'].'</div>'.$GLOBALS['nl']);
			}
		}
	}

	$GLOBALS['fxpglobals']['settings']=$settings_backup;
	$GLOBALS['fxpglobals']['lang']=$lang_backup;

//fxDebug($ea,'$ea: '.$md);
	return $ea;
}

/**
 * ???FUNCTION???
 *
 * @param various $pers  - Mandatory parameter: ???PARAMETER???
 * @param various $sdate - Mandatory parameter: ???PARAMETER???
 * @param various $edate - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getAbsentTasks($pers,$sdate,$edate)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mtasks=array();

	$pers=(int)$pers;
	$sdate=substr($sdate.'00000000',0,8).'000000';
	$edate=substr($edate.'99999999',0,8).'240000';

	$sql  = "SELECT";
	$sql .= " p.projekt_id, p.vorgangsnummer, p.name_projekt, p.projekt_status, p.prioritaet, p.projektmanager, p.projektleiter, p.vertreter, p.aufwand_soll, p.aufwand_ist, p.beschreibung,";
	$sql .= " m.soll_beginn, m.soll_ende, m.auslastung, m.prozent, m.aufwand_std AS paufwand_soll, 0 AS paufwand_ist ";
	$sql .= "FROM";
	$sql .= " projekte p, maplanung m ";
	$sql .= "WHERE";
	$sql .= " m.projekt_id=p.projekt_id AND m.personen_id=".$pers." AND (m.sachmittel_id=0 OR m.sachmittel_id IS NULL) AND m.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$sql .= " AND m.auslastung>0 AND (m.soll_beginn<='".$edate."' AND m.soll_ende>='".$sdate."') AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND p.projekt_status IN (".FXP_PS_PLANNED.",".FXP_PS_ACTIVE.") ";
	$sql .= "ORDER BY";
	$sql .= " p.vorgangsnummer";
	$ta=db_values($sql,'*');
//fxDebug($ta,$sql);
	if(fxIsArray($ta))
	{
		$tcsv='';
		$tarr=array();
		foreach($ta as $k => $td)
		{
			$mtasks[$k]=$td;
			$mtasks[$k]['paufwand_soll']=(int)$td['paufwand_soll'];
			$mtasks[$k]['paufwand_ist']=0;

			if(strlen($tcsv))
				$tcsv .= ", ";
			$tcsv .= $td['projekt_id'];

			$tarr[$td['projekt_id']]=$k;
		}
//fxDebug($tarr,$tcsv);

		// Get actual effort
		$sql  = "SELECT projekt_id, SUM(zeitaufwand) AS ist FROM zeitdaten";
		$sql .= " WHERE personen_id=".$pers." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.") AND projekt_id IN (".$tcsv.")";
		$sql .= " GROUP BY projekt_id";
		$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
		if(fxIsArray($tmp))
		{
			foreach($tmp as $k => $t)
				$mtasks[$tarr[$t['projekt_id']]]['paufwand_ist']=(int)$t['ist'];
		}
	}
//fxDebug($mtasks,'$mtasks');

	return $mtasks;
}

/**
 * ???FUNCTION???
 *
 * @param various $mtasks  - Mandatory parameter: ???PARAMETER???
 * @param various $headers - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function emailAbsentTasks($mtasks, $headers)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$da=array();
	if(fxIsArray($mtasks))
	{
		foreach($mtasks as $c => $t)
		{
			// Task Name|AC~Status|AC~Priority|1st Project Leader|2nd Project Leader|Description|AC~Planned Timespan|AR~Percentage|AR~Planned Effort|AR~Actual Effort|AR~Remain. Effort
			// ...Task Name
			$data='<b>'.no_null($t['vorgangsnummer']).'</b>&nbsp;'.$t['name_projekt'];
			// ...Status
			$data .= '|AC~N~'.meldung($t['projekt_status']);
			// ...Priority
			$data .= '|AC~N~'.meldung($t['prioritaet']);
			// ...1st Project Leader
			$data .= '|N~<b>'.getPersonName($t['projektleiter']).'</b>';
			// ...2nd Project Leader
			$data .= '|N~'.getPersonName($t['vertreter']);
			// ...Description
			$data .= '|'.$t['beschreibung'];
			// ...Planned Timespan
			$pt=get_entry($t['soll_beginn'],'datum');
			$pe=get_entry($t['soll_ende'],'datum');
			if($pe != $pt)
				$pt .= '-'.$pe;
			$data .= '|AC~N~'.$pt;
			// ...Percentage
			$data .= '|AR~N~'.get_entry($t['prozent']*100.0,'dezimal').'&nbsp;%';
			// ...Planned Effort
			$pe=get_entry($t['paufwand_soll']/3600,'dezimal');
			$te=get_entry($t['paufwand_soll']/($t['prozent']*3600),'dezimal');
			$data .= '|AR~N~'.$pe.'&nbsp;<font style="color:#'.colorCode('lightgrey').';">/</font>&nbsp;<font style="color:#'.colorCode('grey').';">'.$te.'</font>&nbsp;h';
			// ...Actual Effort
			if($t['paufwand_ist'] > 0)
				$ac='black';
			else
				$ac='red';
			$data .= '|AR~N~<font style="color:#'.colorCode($ac).';">'.get_entry($t['paufwand_ist']/3600,'dezimal').'</font>&nbsp;h';
			// ...Remain. Effort
			$re=$t['paufwand_soll']-$t['paufwand_ist'];
			if($re < 0)
				$rc='green';
			else if($re > 0)
				$rc='red';
			else
				$rc='black';
			$data .= '|AR~N~<font style="color:#'.colorCode($rc).';">'.get_entry($re/3600,'dezimal').'</font>&nbsp;h';

			$da[$c]=$data;
		}
	}

	$content ='<br /><br />';
	$content .= mailDataTable($headers, $da);

	return $content;
}

/**
 * ???FUNCTION???
 *
 * @param various $pid          - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $return_info  - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $show_message - Optional parameter (default = true):   ???PARAMETER???
 *
 * @return ???RETURN???
 */
function checkDelProject($pid, $return_info=false, $show_message=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pid=(int)$pid;
	$deleteable=true;
	$info='';

	// Get project number, name and main project id
	$sql="SELECT vorgangsnummer, name_projekt, mutterprojekt_id FROM projekte WHERE projekt_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$vna=db_values($sql);
//fxDebug($vna,$sql);
	if(!fxIsArray($vna))
	{
		if(!$return_info)
			return $deleteable;
	}
	$pn='<b class=red>'.no_null($vna['vorgangsnummer']).'</b><span class=red>: '.$vna['name_projekt'].'</span>';
	$sp="SELECT projekt_id FROM projekte WHERE vorgangsnummer LIKE '".$vna['vorgangsnummer']."%' AND mandanten_id=".$GLOBALS['fxpglobals']['client'];

	// Check if there are lower-level processes depending on this project?
	if(!$return_info)
	{
		$sql="SELECT COUNT(projekt_id) AS cnt FROM projekte WHERE elter=".$pid." AND projektart<=".FXP_PRJ_TASK." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$lpc=(int)db_value($sql);
//fxDebug($lpc,$sql);
		if($lpc)
		{
			$deleteable=false;
			if($show_message)
			{
				// 644: This project $var1cannot be deleted, because there are lower-level processes depending on this project!
				error_msg(644, false, '&quot;'.$pn.'&quot; ');
			}
		}
	}

	if($deleteable || $return_info)
	{
		// Main project id and number
		$mpid=(int)$vna['mutterprojekt_id'];
		$mpvn=substr($vna['vorgangsnummer'],0,4);
		$msp="SELECT projekt_id FROM projekte WHERE vorgangsnummer LIKE '".$mpvn."%' AND mandanten_id=".$GLOBALS['fxpglobals']['client'];

		// Arrays for the whole main project
		if(!fxIsArray($GLOBALS['_CDP_arr']))
		{
			$GLOBALS['_CDP_arr']=array(0 => true);

			if($GLOBALS['fxpglobals']['tr'] == 189)	// Matrix needs the whole main project: Is much quicker than each project seperately
			{
				$sql="SELECT projekt_id, elter, projektart, vorgangsnummer, name_projekt FROM projekte WHERE mutterprojekt_id=".$mpid." AND projektart<=".FXP_PRJ_TASK." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY vorgangsnummer";
				$tmp=db_values($sql,'*');
				if(fxIsArray($tmp))
				{
					foreach($tmp as $t)
						$GLOBALS['_CDP_arr'][$t['projekt_id']]=array('elt'=>$t['elter'], 'typ'=>$t['projektart'], 'no'=>$t['vorgangsnummer'], 'nam'=>$t['name_projekt'], 'asm'=>"", 'aem'=>"", 'trc'=>0, 'tec'=>0, 'mtc'=>0, 'mvc'=>0, 'ipc'=>0, 'ioc'=>0, 'adc'=>0);

					// ...time recordings + travel expenses
					$sql="SELECT projekt_id, zeitart, ist_beginn, ist_ende FROM zeitdaten WHERE projekt_id IN (".$msp.") AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.",".FXP_TE_TRAVELEXP.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$tmp=db_values($sql,'*');
					if(fxIsArray($tmp))
					{
						foreach($tmp as $t)
						{
							$tpid=$t['projekt_id'];
							if(($t['zeitart'] == FXP_TE_TIMEREC) || ($t['zeitart'] == FXP_TE_TIMEREC_SUM) || ($t['zeitart'] == FXP_TE_TRAVELEXP_TIMEREC))	// time recordings
								$trc=1;
							else
								$trc=0;
							if(($t['zeitart'] == FXP_TE_TRAVELEXP) || ($t['zeitart'] == FXP_TE_TRAVELEXP_TIMEREC))	// travel expenses
								$tec=1;
							else
								$tec=0;

							while(isset($GLOBALS['_CDP_arr'][$tpid]))
							{
								$GLOBALS['_CDP_arr'][$tpid]['trc'] += $trc;
								$GLOBALS['_CDP_arr'][$tpid]['tec'] += $tec;

								if(!strlen($GLOBALS['_CDP_arr'][$tpid]['asm']) || ($t['ist_beginn'] < $GLOBALS['_CDP_arr'][$tpid]['asm']))
									$GLOBALS['_CDP_arr'][$tpid]['asm']=$t['ist_beginn'];
								if(!strlen($GLOBALS['_CDP_arr'][$tpid]['aem']) || ($t['ist_ende'] > $GLOBALS['_CDP_arr'][$tpid]['aem']))
									$GLOBALS['_CDP_arr'][$tpid]['aem']=$t['ist_ende'];

								$tpid=$GLOBALS['_CDP_arr'][$tpid]['elt'];
								if($tpid <= 0)
									break;
							}
						}
					}

					// ...instant materials
					$sql="SELECT fuervorgang FROM sachmittel WHERE fuervorgang IN (".$msp.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$tmp=db_values($sql,'*');
					if(fxIsArray($tmp))
					{
						foreach($tmp as $t)
						{
							$tpid=$t['fuervorgang'];
							while(isset($GLOBALS['_CDP_arr'][$tpid]))
							{
								$GLOBALS['_CDP_arr'][$tpid]['mtc']++;

								$tpid=$GLOBALS['_CDP_arr'][$tpid]['elt'];
								if($tpid <= 0)
									break;
							}
						}
					}

					// ...material usages
					$sql="SELECT projekt_id FROM materialverbrauch WHERE projekt_id IN (".$msp.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$tmp=db_values($sql,'*');
					if(fxIsArray($tmp))
					{
						foreach($tmp as $t)
						{
							$tpid=$t['projekt_id'];
							while(isset($GLOBALS['_CDP_arr'][$tpid]))
							{
								$GLOBALS['_CDP_arr'][$tpid]['mvc']++;

								$tpid=$GLOBALS['_CDP_arr'][$tpid]['elt'];
								if($tpid <= 0)
									break;
							}
						}
					}

					// ...incoming invoices (paid/open)
					$sql="SELECT projekt_id, bezahlt_am FROM rechnungseingang WHERE projekt_id IN (".$msp.") AND nettobetrag>0 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$tmp=db_values($sql,'*');
					if(fxIsArray($tmp))
					{
						foreach($tmp as $t)
						{
							$tpid=$t['projekt_id'];
							while(isset($GLOBALS['_CDP_arr'][$tpid]))
							{
								if($t['bezahlt_am'])
									$GLOBALS['_CDP_arr'][$tpid]['ipc']++;
								else
									$GLOBALS['_CDP_arr'][$tpid]['ioc']++;

								$tpid=$GLOBALS['_CDP_arr'][$tpid]['elt'];
								if($tpid <= 0)
									break;
							}
						}
					}

					// ...active documents
					$sql="SELECT elter FROM d_dokumente WHERE elter IN (".$msp.") AND elter_art=3 AND (archiv=0 OR archiv IS NULL) AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$tmp=db_values($sql,'*');
					if(fxIsArray($tmp))
					{
						foreach($tmp as $t)
						{
							$tpid=$t['elter'];
							while(isset($GLOBALS['_CDP_arr'][$tpid]))
							{
								$GLOBALS['_CDP_arr'][$tpid]['adc']++;

								$tpid=$GLOBALS['_CDP_arr'][$tpid]['elt'];
								if($tpid <= 0)
									break;
							}
						}
					}
				}
			}
		}

		// Get task data
		$msg='';
		// ...0: time recordings
		if(fxIsArray($GLOBALS['_CDP_arr'][$pid]))
			$trc=$GLOBALS['_CDP_arr'][$pid]['trc'];
		else
		{
			$sql="SELECT COUNT(zeit_id) AS cnt FROM zeitdaten WHERE projekt_id IN (".$sp.") AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$trc=(int)db_value($sql);
//fxDebug($trc,$sql);
		}
		$info .= $trc;
		if($trc)
			$msg .= '<li>'.meldung(3107).'</li>';	// 3107: bereits Zeiten darauf erfasst wurden / times have been already recorded to it
		// ...1: travel expenses
		if(fxIsArray($GLOBALS['_CDP_arr'][$pid]))
			$tec=$GLOBALS['_CDP_arr'][$pid]['tec'];
		else
		{
			$sql="SELECT COUNT(zeit_id) AS cnt FROM zeitdaten WHERE projekt_id IN (".$sp.") AND zeitart IN (".FXP_TE_TRAVELEXP.",".FXP_TE_TRAVELEXP_TIMEREC.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$tec=(int)db_value($sql);
//fxDebug($tec,$sql);
		}
		$info .= ','.$tec;
		if($tec)
			$msg .= '<li>'.meldung(3108).'</li>';	// 3108: bereits Reisekosten darauf gebucht wurden / travel expenses have been already booked to it
		// ...2: instant materials
		if(fxIsArray($GLOBALS['_CDP_arr'][$pid]))
			$mtc=$GLOBALS['_CDP_arr'][$pid]['mtc'];
		else
		{
			$sql="SELECT COUNT(sachmittel_id) AS cnt FROM sachmittel WHERE fuervorgang IN (".$sp.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$mtc=(int)db_value($sql);
//fxDebug($mtc,$sql);
		}
		$info .= ','.$mtc;
		if($mtc)
			$msg .= '<li>'.meldung(3109).'</li>';	// 3109: bereits Sofortmaterial zugeordnet wurde / instant material has been already assigned to it
		// ...3: material usages
		if(fxIsArray($GLOBALS['_CDP_arr'][$pid]))
			$mvc=$GLOBALS['_CDP_arr'][$pid]['mvc'];
		else
		{
			$sql="SELECT COUNT(id) AS cnt FROM materialverbrauch WHERE projekt_id IN (".$sp.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$mvc=(int)db_value($sql);
//fxDebug($mvc,$sql);
		}
		$info .= ','.$mvc;
		if($mvc)
			$msg .= '<li>'.meldung(3207).'</li>';	// 3207: bereits Materialverbrauch darauf gebucht wurde / material usages have been already booked to it
		// ...used budget / outgoing invoices
		if(!$return_info)
		{
			$sql  = "SELECT SUM(budget_pers_int_verbr) AS ihrused, SUM(budget_mat_int_verbr) AS imrused, SUM(budget_pers_ext_abger) AS ehrinv, SUM(budget_mat_ext_abger) AS emrinv";
			$sql .= " FROM budget_summe WHERE projekt_id IN (".$sp.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$bda=db_values($sql);
//fxDebug($bda,$sql);
			if(get_double($bda['ihrused']) || get_double($bda['imrused']))
				$msg .= '<li>'.meldung(3110).'</li>';	// 3110: bereits internes Budget dafür verbraucht wurde / internal budget has been already used on it
			if(get_double($bda['ehrinv']) || get_double($bda['emrinv']))
				$msg .= '<li>'.meldung(3111).'</li>';	// 3111: bereits Ausgangsrechnungen dafür erstellt wurden / outgoing invoices have been already created for it
		}
		// ...4+5: incoming invoices (paid/open)
		if(fxIsArray($GLOBALS['_CDP_arr'][$pid]))
		{
			$ipc=$GLOBALS['_CDP_arr'][$pid]['ipc'];
			$ioc=$GLOBALS['_CDP_arr'][$pid]['ioc'];
		}
		else
		{
			$sql="SELECT COUNT(rechnungs_id) AS cnt FROM rechnungseingang WHERE projekt_id IN (".$sp.") AND nettobetrag>0 AND bezahlt_am IS NOT NULL AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$ipc=(int)db_value($sql);
//fxDebug($ipc,$sql);
			$sql="SELECT COUNT(rechnungs_id) AS cnt FROM rechnungseingang WHERE projekt_id IN (".$sp.") AND nettobetrag>0 AND bezahlt_am IS NULL AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$ioc=(int)db_value($sql);
//fxDebug($ioc,$sql);
		}
		$info .= ','.$ipc.','.$ioc;
		if($ipc || $ioc)
			$msg .= '<li>'.meldung(3112).'</li>';	// 3112: bereits Eingangsrechnung dafür zugeordnet wurden / incoming invoices have been already assigned to it
		// ...6: active documents
		if(fxIsArray($GLOBALS['_CDP_arr'][$pid]))
			$adc=$GLOBALS['_CDP_arr'][$pid]['adc'];
		else
		{
			$sql="SELECT COUNT(dok_id) AS cnt FROM d_dokumente WHERE elter IN (".$sp.") AND elter_art=3 AND (archiv=0 OR archiv IS NULL) AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$adc=(int)db_value($sql);
//fxDebug($adc,$sql);
		}
		$info .= ','.$adc;
		if($adc)
			$msg .= '<li>'.meldung(3113).'</li>';	// 3113: aktive Dokumente damit verbunden sind / active documents are linked to it

		if(strlen($msg))
		{
			$deleteable=false;
			if($show_message)
			{
				// 3106: Diese Aufgabe $var1kann nicht gelöscht werden, da$var2 / This task $var1cannot be deleted, because$var2
				error_msg(3106, false, '&quot;'.$pn.'&quot; ', ':<br /><br /><ul>'.$msg.'</ul>');
			}
		}
	}

	if($return_info)
		return $info;
	return $deleteable;
}

/**
 * ???FUNCTION???
 *
 * @param various $pid  - Mandatory parameter:                ???PARAMETER???
 * @param integer $wrap - Optional parameter (default = 32):  ???PARAMETER???
 */
function addProjectToArray($pid, $wrap=32)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!isset($GLOBALS['pra']))
		$GLOBALS['pra']=array();
	if(!isset($GLOBALS['pra'][$ti]))
	{
		if(!isset($GLOBALS['cv']))
			$GLOBALS['cv']=darf_kosten_sehen(0);	// 0=none, 1=int, 2=ext, 3=all
		$pc=$GLOBALS['cv'];
		if($pid > 0)
		{
			$sql="SELECT vorgangsnummer,name_projekt FROM projekte WHERE projekt_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$tmp=db_values($sql);
			if(fxIsArray($tmp))
			{
				$no=$tmp['vorgangsnummer'];
				$pn='<table border=0 cellpadding=0 cellspacing=0><tr><td nowrap valign=top><b class=darkgrey>'.no_null($tmp['vorgangsnummer']).'</b>&nbsp;</td><td valign=top><font class=grey>'.str_replace('"','&quot;',text_wrap($tmp['name_projekt'],true,$wrap)).'</font></td></tr></table>';
				if($pc > 0)
					$pc=darf_kosten_sehen($pid);	// 0=none, 1=int, 2=ext, 3=all
			}
			else
			{
				$no='B';
				if(!isset($GLOBALS['txt_noallocation']))
					$GLOBALS['txt_noallocation']='<i class=lightergrey>'.meldung(597,false).'</i>';	// No Allocation
				$pn=$GLOBALS['txt_noallocation'];
			}
		}
		else
		{
			$no='A';
			if(!isset($GLOBALS['txt_unknown']))
				$GLOBALS['txt_unknown']='<i class=lightergrey>'.meldung(1449,false).'</i>';	// [Unknown]
			$pn=$GLOBALS['txt_unknown'];
		}
		$GLOBALS['pra'][$pid]=array('no'=>$no, 'pn'=>$pn, 'pc'=>$pc);
	}
}

/**
 * ???FUNCTION???
 *
 * @param various $pid - Mandatory parameter: ???PARAMETER???
 */
function getPrIA($pid)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!isset($GLOBALS['pra'][$pid]))
	{
		if($pid)
		{
			$sql  = "SELECT p.vorgangsnummer,p.name_projekt,b.abrechnungsart_mat FROM projekte p, budget_summe b";
			$sql .= " WHERE p.projekt_id=".$pid." AND b.projekt_id=p.projekt_id AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND b.mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$tmp=db_values($sql);
//fxDebug($tmp,$sql);
		}
		else
			$tmp=false;
		if(fxIsArray($tmp))
		{
			$pn='<b>'.no_null($tmp['vorgangsnummer']).'</b> '.fxHtmlEncode($tmp['name_projekt']);
			$it=(int)$tmp['abrechnungsart_mat'];
		}
		else
		{
			$pn=$GLOBALS['txt_noallocation'];
			$it=FXP_INV_NOT;
		}
		$GLOBALS['pra'][$pid]=array('name'=>$pn, 'itype'=>$it);
	}
}

/**
 * Copy a project
 *
 * @param various $src_id     - Mandatory parameter:                   ???PARAMETER???
 * @param various $dst_id     - Mandatory parameter:                   ???PARAMETER???
 * @param integer $inf        - Optional parameter (default = 0):      ???PARAMETER???
 * @param integer $res        - Optional parameter (default = 0):      ???PARAMETER???
 * @param integer $tad        - Optional parameter (default = 0):      ???PARAMETER???
 * @param integer $timeadjust - Optional parameter (default = 0):      ???PARAMETER???
 * @param integer $mand       - Optional parameter (default = 0):      ???PARAMETER???
 * @param boolean $addinfo    - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $dbaccess   - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return Error code on error or false on success
 */
function copy_project($src_id, $dst_id, $inf=0, $res=0, $tad=0, $timeadjust=0, $mand=0, $addinfo=false, $dbaccess=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$src_id=(int)$src_id;
	$dst_id=(int)$dst_id;
	if(!$src_id)
		return 1837;	// Es wurde kein Quell-Vorgang ausgewählt!

	$mand=(int)$mand;
	if(!$mand)
		$mand=$GLOBALS['fxpglobals']['client'];
	if(!$tad)
		$tad=0;

	// Prefix
	getAllPrefixes();
//fxDebug($GLOBALS['mp_prefixes'],'mp_prefixes');

	// Max. Feldlänge des Projektnamens
	$maxpn_len=(int)db_value("SELECT laenge FROM felder WHERE id=46 AND mandanten_id=0");

	$dids="";
	if($dst_id)
		$dids=superior_vn($dst_id, true);

	// Array erstellen, falls noch nicht vorhanden oder sich Parameter geändert haben oder die zeiten angepasst werden sollen
	if(!is_array($GLOBALS['_cppj']) || !is_array($GLOBALS['_cppj']['info']) || ($GLOBALS['_cppj']['info']['sid'] != $src_id) || ($GLOBALS['_cppj']['info']['did'] != $dst_id) || ($GLOBALS['_cppj']['info']['inf'] != $inf) || ($GLOBALS['_cppj']['info']['tad'] != $tad) || $timeadjust)
	{
		if(is_array($GLOBALS['_cppj']))
			unset($GLOBALS['_cppj']);

		// Quelle-IDs
		if($inf)
			$sids=inferior_vn($src_id, 3);
		else
			$sids=$src_id;

		// Info-Block
		$GLOBALS['_cppj']=array
		(
			'info' => array
			(
				'sid'	=> $src_id,
				'did'	=> $dst_id,
				'inf'	=> $inf,
				'tad'	=> $tad,
				'time'	=> $timeadjust,

				'pm'	=> 0,

				'smpid'	=> 0,
				'dmpid'	=> 0,

				'sdp'	=> 0,
				'ddp'	=> 0,

				'dsb'	=> '',
				'dse'	=> '',
				'ssb'	=> '',
				'sse'	=> '',
				'nsb'	=> '',
				'nse'	=> '',

				'spfx'	=> '',
				'dpfx'	=> '',
				'spfxl'	=> 0,
				'dpfxl'	=> 0,

				'dbrpi'	=> 0.00,
				'dbrpe'	=> 0.00,
				'dbrmi'	=> 0.00,
				'dbrme'	=> 0.00,
				'dbpig'	=> 0.00,
				'dbpeg'	=> 0.00,
				'dbmig'	=> 0.00,
				'dbmeg'	=> 0.00,
				'sbpig'	=> 0.00,
				'sbpeg'	=> 0.00,
				'sbmig'	=> 0.00,
				'sbmeg'	=> 0.00,
				'nbpig'	=> 0.00,
				'nbpeg'	=> 0.00,
				'nbmig'	=> 0.00,
				'nbmeg'	=> 0.00
			),
			'data' => array()
		);

		// Prefix
		$GLOBALS['_cppj']['info']['smpid']=(int)db_value("SELECT mutterprojekt_id FROM projekte WHERE projekt_id=".$src_id." AND mandanten_id=".$mand);
		if(fxIsArray($GLOBALS['mp_prefixes']) && isset($GLOBALS['mp_prefixes'][$GLOBALS['_cppj']['info']['smpid']]))
			$GLOBALS['_cppj']['info']['spfx']=$GLOBALS['mp_prefixes'][$GLOBALS['_cppj']['info']['smpid']];
		$GLOBALS['_cppj']['info']['spfxl']=strlen($GLOBALS['_cppj']['info']['spfx']);
		if($dst_id)
		{
			$GLOBALS['_cppj']['info']['dmpid']=(int)db_value("SELECT mutterprojekt_id FROM projekte WHERE projekt_id=".$dst_id." AND mandanten_id=".$mand);
			$GLOBALS['_cppj']['info']['pm']=(int)db_value("SELECT projektmanager FROM projekte WHERE projekt_id=".$GLOBALS['_cppj']['info']['dmpid']." AND mandanten_id=".$mand);
			if(fxIsArray($GLOBALS['mp_prefixes']) && isset($GLOBALS['mp_prefixes'][$GLOBALS['_cppj']['info']['dmpid']]))
				$GLOBALS['_cppj']['info']['dpfx']=$GLOBALS['mp_prefixes'][$GLOBALS['_cppj']['info']['dmpid']];
		}
		else
			$GLOBALS['_cppj']['info']['dpfx']=$GLOBALS['_cppj']['info']['spfx'];
		$GLOBALS['_cppj']['info']['dpfxl']=strlen($GLOBALS['_cppj']['info']['dpfx']);
//echo('smpid='.$GLOBALS['_cppj']['info']['smpid'].' (prefix=&quot;'.$GLOBALS['_cppj']['info']['spfx'].'&quot;), dmpid='.$GLOBALS['_cppj']['info']['dmpid'].' (prefix=&quot;'.$GLOBALS['_cppj']['info']['dpfx'].'&quot;)<hr />');

		// Daten-Block
		$elt=0;
		if($dst_id && strlen($dids))
		{
			// Int. und Ext. Budgetrahmen (Pers. + Mat.) + Int. und Ext. geplante Budgets (Pers. + Mat.) [Destination]
			$sql  = "SELECT budgetrahmen_pers_int, budgetrahmen_pers_ext, budgetrahmen_mat_int, budgetrahmen_mat_ext,";
			$sql .= " budget_pers_int_gepl, budget_pers_ext_gepl, budget_mat_int_gepl, budget_mat_ext_gepl";
			$sql .= " FROM budget_summe WHERE projekt_id=".$dst_id." AND mandanten_id=".$mand;
			$bdt=db_values($sql);
			$GLOBALS['_cppj']['info']['dbrpi']=(double)$bdt['budgetrahmen_pers_int'];
			$GLOBALS['_cppj']['info']['dbrpe']=(double)$bdt['budgetrahmen_pers_ext'];
			$GLOBALS['_cppj']['info']['dbrmi']=(double)$bdt['budgetrahmen_mat_ext'];
			$GLOBALS['_cppj']['info']['dbrme']=(double)$bdt['budgetrahmen_mat_int'];
			$GLOBALS['_cppj']['info']['dbpig']=(double)$bdt['budget_pers_int_gepl'];
			$GLOBALS['_cppj']['info']['dbpeg']=(double)$bdt['budget_pers_ext_gepl'];
			$GLOBALS['_cppj']['info']['dbmig']=(double)$bdt['budget_mat_int_gepl'];
			$GLOBALS['_cppj']['info']['dbmeg']=(double)$bdt['budget_mat_ext_gepl'];

			$sql  = "SELECT p.projekt_id, p.elter, p.vorgangsnummer, p.name_projekt, p.projektart, p.abhaengigkeit, z.soll_beginn, z.soll_ende";
			$sql .= " FROM projekte p, zeitdaten z";
			$sql .= " WHERE z.projekt_id=p.projekt_id AND z.zeitart=102 AND p.projekt_id IN (".$dids.") AND p.mandanten_id=".$mand." AND z.mandanten_id=".$mand;
			$sql .= " ORDER BY p.vorgangsnummer";
			$dst_inf=db_values($sql, '*');
//fxDebug($dst_inf,$dst_id, 0);
			if(fxIsArray($dst_inf))
			{
				foreach($dst_inf as $dc => $d)
				{
					$id=(int)$d['projekt_id'];
					$np=$d['name_projekt'];
					if($GLOBALS['_cppj']['info']['dpfxl'] && (substr($np,0,$GLOBALS['_cppj']['info']['dpfxl']) == $GLOBALS['_cppj']['info']['dpfx']))
						$np=trim(substr($np,$GLOBALS['_cppj']['info']['dpfxl']));
					$GLOBALS['_cppj']['data'][$id]=array
					(
						'md'	=> 1,
						'id'	=> $id,
						'elt'	=> (int)$d['elter'],
						'art'	=> (int)$d['projektart'],
						'dpp'	=> (int)$d['abhaengigkeit'],
						'dps'	=> 0,
						'ovn'	=> $d['vorgangsnummer'],
						'nvn'	=> $d['vorgangsnummer'],
						'opn'	=> $np,
						'npn'	=> $np,
						'osb'	=> substr($d['soll_beginn'], 0, 8).'000000',
						'nsb'	=> substr($d['soll_beginn'], 0, 8).'000000',
						'ose'	=> substr($d['soll_ende'], 0, 8).'240000',
						'nse'	=> substr($d['soll_ende'], 0, 8).'240000'
					);
					if($GLOBALS['_cppj']['data'][$id]['dpp'])
					{
						if(is_array($GLOBALS['_cppj']['data'][$GLOBALS['_cppj']['data'][$id]['dpp']]))
							$GLOBALS['_cppj']['data'][$GLOBALS['_cppj']['data'][$id]['dpp']]['dps']=$id;
						$GLOBALS['_cppj']['info']['ddp']=1;
					}
					$elt=$id;
				}
				$GLOBALS['_cppj']['info']['dsb']=$GLOBALS['_cppj']['data'][$dst_id]['nsb'];
				$GLOBALS['_cppj']['info']['dse']=$GLOBALS['_cppj']['data'][$dst_id]['nse'];
			}
		}
		else
		{
			$GLOBALS['_cppj']['info']['dsb']='00000000000000';
			$GLOBALS['_cppj']['info']['dse']='99999999240000';
		}

		// Int. + Ext. geplante Budgets (Pers. + Mat.) [Source]
		$sql="SELECT budget_pers_int_gepl, budget_pers_ext_gepl, budget_mat_int_gepl, budget_mat_ext_gepl FROM budget_summe WHERE projekt_id=".$src_id." AND mandanten_id=".$mand;
		$bdt=db_values($sql);
		$GLOBALS['_cppj']['info']['sbpig']=(double)$bdt['budget_pers_int_gepl'];
		$GLOBALS['_cppj']['info']['sbpeg']=(double)$bdt['budget_pers_ext_gepl'];
		$GLOBALS['_cppj']['info']['sbmig']=(double)$bdt['budget_mat_int_gepl'];
		$GLOBALS['_cppj']['info']['sbmeg']=(double)$bdt['budget_mat_ext_gepl'];
		$GLOBALS['_cppj']['info']['nbpig']=$GLOBALS['_cppj']['info']['dbpig']+$GLOBALS['_cppj']['info']['sbpig'];
		$GLOBALS['_cppj']['info']['nbpeg']=$GLOBALS['_cppj']['info']['dbpeg']+$GLOBALS['_cppj']['info']['sbpeg'];
		$GLOBALS['_cppj']['info']['nbmig']=$GLOBALS['_cppj']['info']['dbmig']+$GLOBALS['_cppj']['info']['sbmig'];
		$GLOBALS['_cppj']['info']['nbmeg']=$GLOBALS['_cppj']['info']['dbmeg']+$GLOBALS['_cppj']['info']['sbmeg'];

		$sql  = "SELECT p.projekt_id, p.elter, p.vorgangsnummer, p.name_projekt, p.projektart, p.abhaengigkeit, z.soll_beginn, z.soll_ende";
		$sql .= " FROM projekte p, zeitdaten z";
		$sql .= " WHERE z.projekt_id=p.projekt_id AND z.zeitart=102 AND p.projekt_id IN (".$sids.") AND p.mandanten_id=".$mand." AND z.mandanten_id=".$mand;
		$sql .= " ORDER BY p.vorgangsnummer";
		$src_inf=db_values($sql, '*');
//fxDebug($src_inf,$sql, 0);
		if(fxIsArray($src_inf))
		{
			// Neue Vorgangsnummer
			$vn_start=$src_inf[0]['vorgangsnummer'];
			if(fxIsArray($dst_inf) && $dst_id)
			{
				$sql="SELECT MAX(vorgangsnummer) FROM projekte WHERE elter=".$dst_id." AND mandanten_id=".$mand;
				$vn_neu=db_value($sql);
				if(strlen($vn_neu))
				{
					$ln=(int)substr($vn_neu, -4)+1;
					$vn_neu=substr($vn_neu, 0, -5).'.'.substr('0000'.$ln, -4);
				}
				else
				{
					$ln=0;
					if(strtolower(substr($GLOBALS['fxpglobals']['settings']['vorgang0'], 0, 1)) == 'n')
						$ln=1;
					$vn_neu=$dst_inf[sizeof($dst_inf)-1]['vorgangsnummer'].'.'.substr('0000'.$ln, -4);
				}
			}
			else
			{
				$sql="SELECT MAX(vorgangsnummer) FROM projekte WHERE projektart=".FXP_PRJ_MAIN." AND mandanten_id=".$mand;
				$vn_neu=db_value($sql);
				if(strlen($vn_neu))
					$vn_neu=substr('0000'.((int)$vn_neu+1), -4);
				else
				{
					$ln=0;
					if(strtolower(substr($GLOBALS['fxpglobals']['settings']['vorgang0'], 0, 1)) == 'n')
						$ln=1;
					$vn_neu=substr('0000'.$ln, -4);
				}
			}

			foreach($src_inf as $dc => $d)
			{
				$copyzusatz='';

				$ido=(int)$d['projekt_id'];
				$id='N'.$ido;
				$art=(int)$d['projektart'];

				$np=$d['name_projekt'];
				if($GLOBALS['_cppj']['info']['spfxl'] && (substr($np,0,$GLOBALS['_cppj']['info']['spfxl']) == $GLOBALS['_cppj']['info']['spfx']))
					$np=trim(substr($np,$GLOBALS['_cppj']['info']['spfxl']));

				if(!$dc)
				{
					$copyzusatz=' (*copy)';
					if((strlen($np)+strlen($copyzusatz)+$GLOBALS['_cppj']['info']['spfxl']) > $maxpn_len)
						$copyzusatz=' (c)';
					if((strlen($np)+strlen($copyzusatz)+$GLOBALS['_cppj']['info']['spfxl']) > $maxpn_len)
						$copyzusatz='';

					if(!$elt)						// Kein Vorgänger, dann Mutterprojekt
						$art=FXP_PRJ_MAIN;
					else if($art == FXP_PRJ_MAIN)	// Mutterprojekt mit Vorgänger, dann Teilprojekt
						$art=FXP_PRJ_SUB;
				}
				else
					$elt='N'.(int)$d['elter'];
				$vn=$vn_neu.substr($d['vorgangsnummer'], strlen($vn_start));

				// Ressourcen
				$pr=false;
				if($res && ($art == FXP_PRJ_TASK))
				{
					$sql="SELECT personen_id FROM maplanung WHERE personen_id IS NOT NULL AND personen_id>0 AND projekt_id=".$ido." AND mandanten_id=".$mand;
					if(db_value($sql))
						$pr=true;
				}
				$ps=false;
				if($res && ($art == FXP_PRJ_TASK))
				{
					$sql="SELECT sachmittel_id FROM maplanung WHERE sachmittel_id IS NOT NULL AND sachmittel_id>0 AND projekt_id=".$ido." AND mandanten_id=".$mand;
					if(db_value($sql))
						$ps=true;
				}
				$GLOBALS['_cppj']['data'][$id]=array
				(
					'md'	=> 2,
					'id'	=> $id,
					'elt'	=> $elt,
					'art'	=> $art,
					'dpp'	=> (int)$d['abhaengigkeit'],
					'dps'	=> 0,
					'res'	=> $pr,
					'sam'	=> $ps,
					'ovn'	=> $d['vorgangsnummer'],
					'nvn'	=> $vn,
					'opn'	=> $np,
					'npn'	=> substr($np.$copyzusatz, 0, $maxpn_len-$GLOBALS['_cppj']['info']['spfxl']),
					'osb'	=> substr($d['soll_beginn'], 0, 8).'000000',
					'nsb'	=> substr(time_add($d['soll_beginn'], $tad, 'D'), 0, 8).'000000',
					'ose'	=> substr($d['soll_ende'], 0, 8).'240000',
					'nse'	=> substr(time_add($d['soll_ende'], $tad, 'D'), 0, 8).'240000'
				);
				if($GLOBALS['_cppj']['data'][$id]['dpp'])
				{
					if(is_array($GLOBALS['_cppj']['data'][$GLOBALS['_cppj']['data'][$id]['dpp']]))
						$GLOBALS['_cppj']['data'][$GLOBALS['_cppj']['data'][$id]['dpp']]['dps']=$id;
					$GLOBALS['_cppj']['info']['sdp']=1;
				}
				if(!strlen($GLOBALS['_cppj']['info']['ssb']) || ($GLOBALS['_cppj']['info']['ssb'] > $GLOBALS['_cppj']['data'][$id]['osb']))
					$GLOBALS['_cppj']['info']['ssb']=$GLOBALS['_cppj']['data'][$id]['osb'];
				if(!strlen($GLOBALS['_cppj']['info']['sse']) || ($GLOBALS['_cppj']['info']['sse'] < $GLOBALS['_cppj']['data'][$id]['ose']))
					$GLOBALS['_cppj']['info']['sse']=$GLOBALS['_cppj']['data'][$id]['ose'];
				if(!strlen($GLOBALS['_cppj']['info']['nsb']) || ($GLOBALS['_cppj']['info']['nsb'] > $GLOBALS['_cppj']['data'][$id]['nsb']))
					$GLOBALS['_cppj']['info']['nsb']=$GLOBALS['_cppj']['data'][$id]['nsb'];
				if(!strlen($GLOBALS['_cppj']['info']['nse']) || ($GLOBALS['_cppj']['info']['nse'] < $GLOBALS['_cppj']['data'][$id]['nse']))
					$GLOBALS['_cppj']['info']['nse']=$GLOBALS['_cppj']['data'][$id]['nse'];
			}
		}

		// Zeit anpassen, und zwar Top-Down
		if($GLOBALS['_cppj']['info']['did'] && fxIsArray($GLOBALS['_cppj']['data']) && ($timeadjust == 1))
		{
			foreach($GLOBALS['_cppj']['data'] as $id => $d)
			{
				if(($d['md'] != 1) && ($d['md'] != 3))
				{
					$pvn=substr($d['nvn'],0,-5);
					if(strlen($pvn))
					{
						foreach($GLOBALS['_cppj']['data'] as $pid => $dd)
						{
							if(($dd['nvn'] == $pvn) && (($d['nsb'] < $dd['nsb']) || ($d['nse'] > $dd['nse'])))
							{
//echo('$id='.$id.' ('.$d['nvn'].'): $pid='.$pid.' ('.$pvn.') -- nsb: '.$d['nsb'].' ('.$dd['nsb'].'), nse: '.$d['nse'].' ('.$dd['nse'].')<br />');
								if(($d['nsb'] < $dd['nsb']) && ($d['nse'] > $dd['nse']))	// Komplett ausserhalb Elterzeitraum
								{
									$GLOBALS['_cppj']['data'][$id]['nsb']=$dd['nsb'];
									$GLOBALS['_cppj']['data'][$id]['nse']=$dd['nse'];
								}
								else
								{
									$fa1=ceil(time_diff($dd['nsb'],$dd['nse'],'D'));
									$fa2=ceil(time_diff($dd['osb'],$dd['ose'],'D'));
									$fa=$fa1 / $fa2;
									$ta=round(time_diff($GLOBALS['_cppj']['info']['ssb'],$d['osb'],'D') * $fa);
									$tb=round(time_diff($dd['osb'],$d['osb'],'D') * $fa);
									$td=max(0, round(time_diff($d['osb'],$d['ose'],'D') * $fa) - 1);
//echo('-&gt; fa1:'. $fa1.', fa2: '.$fa2.' = fa: '.$fa.' -- ta: '.$ta.', tb: '.$tb.', td: '.$td.'<br />');
									if($dd['md'] == 2)
									{
										$GLOBALS['_cppj']['data'][$id]['nsb']=min($dd['nse'], max($dd['nsb'], substr(time_add($dd['nsb'],$tb,'D'),0,8).'000000'));
										$GLOBALS['_cppj']['data'][$id]['nse']=max($GLOBALS['_cppj']['data'][$id]['nsb'], min($dd['nse'], substr(time_add($GLOBALS['_cppj']['data'][$id]['nsb'],$td,'D'),0,8).'240000'));
									}
									else if($d['nsb'] < $dd['nsb'])							// Startdatum vor Elterstart
									{
										$GLOBALS['_cppj']['data'][$id]['nsb']=min($dd['nse'], max($dd['nsb'], substr(time_add($dd['nsb'],$ta,'D'),0,8).'000000'));
										$GLOBALS['_cppj']['data'][$id]['nse']=max($GLOBALS['_cppj']['data'][$id]['nsb'], min($dd['nse'], substr(time_add($GLOBALS['_cppj']['data'][$id]['nsb'],$td,'D'),0,8).'240000'));
									}
									else													// Enddatum hinter Elterende
									{
										$GLOBALS['_cppj']['data'][$id]['nse']=min($dd['nse'], max($dd['nsb'], substr(time_add($dd['nse'],-$ta,'D'),0,8).'240000'));
										$GLOBALS['_cppj']['data'][$id]['nsb']=max($dd['nsb'], min($GLOBALS['_cppj']['data'][$id]['nse'], substr(time_add($GLOBALS['_cppj']['data'][$id]['nse'],-$td,'D'),0,8).'000000'));
									}
								}
//echo('-&gt; nsb: '.$GLOBALS['_cppj']['data'][$id]['nsb'].', nse: '.$GLOBALS['_cppj']['data'][$id]['nse'].')<hr />');
								break;
							}
						}
					}
				}
			}
		}
	}

	// Zeit anpassen, und zwar Bottom-Up
	if(fxIsArray($GLOBALS['_cppj']['data']) && ($timeadjust == 2))
	{
		foreach($GLOBALS['_cppj']['data'] as $id => $d)
		{
			if(($d['md'] == 1) || ($d['md'] == 3))
			{
				$GLOBALS['_cppj']['data'][$id]['nsb']=min($d['nsb'],$GLOBALS['_cppj']['info']['nsb']);
				$GLOBALS['_cppj']['data'][$id]['nse']=max($d['nse'],$GLOBALS['_cppj']['info']['nse']);

				$GLOBALS['_cppj']['info']['dsb']=min($GLOBALS['_cppj']['info']['dsb'],$GLOBALS['_cppj']['data'][$id]['nsb']);
				$GLOBALS['_cppj']['info']['dse']=max($GLOBALS['_cppj']['info']['dse'],$GLOBALS['_cppj']['data'][$id]['nse']);
			}
		}
	}

	// Datenbank-Calls
	$err=false;
	if($dbaccess)
	{
		if(fxIsArray($GLOBALS['_cppj']['data']))
		{
//fxDebug($addinfo, '$addinfo');

			$pdat=projektinfo($src_id, false);
//fxDebug($pdat, $src_id);

			$fid=0;
			$fidp=0;
			$prelist='';
			$eltlist=array();
			$pia=array();
			$jetzt=fxNow();

			foreach($GLOBALS['_cppj']['data'] as $id => $d)
			{
				if(substr($id, 0, 1) == 'N')
					$id=(int)substr($id, 1);

				if(!$d['md'])
					continue;
				else if($d['md'] == 1)
				{
					if($prelist)
						$prelist .= ',';
					$prelist .= $id;
					continue;
				}
				else if($d['md'] == 3)
				{
//echo('<B>UPDATE:</B> '.$d['nvn'].': "'.$d['npn'].'"<BR>');
					if($prelist)
						$prelist .= ',';
					$prelist .= $id;

					// UPDATE: Tabelle "projekte"
					$sql="UPDATE zeitdaten SET soll_beginn='".$d['nsb']."', soll_ende='".$d['nse']."' WHERE projekt_id=".$id." AND zeitart=102 AND mandanten_id=".$mand;
					$err=db_x($sql);
//fxDebug($err, $sql);
					if($err)
						$err=1851;	// Die Projektdaten eines Vorgangs konnten nicht angepasst werden! Der Kopierprozess wurde abgebrochen.
				}
				else if(!fxIsArray($pdat) || !fxIsArray($pdat[$id]) || !fxIsArray($pdat[$id]['data']))
					$err=1840;	// Die Quelldaten für einen Vorgang konnten nicht ermittelt werden?
				else if($d['md'] == 2)
				{
//echo('<B>INSERT:</B> '.$d['nvn'].': "'.$d['npn'].'"<BR>');
					// INSERT: Tabelle "projekte"
					$idp=id_max('projekte','projekt_id');
					$idz=id_max('zeitdaten','zeit_id');
					if((int)$d['art'] == FXP_PRJ_MAIN)
						$GLOBALS['_cppj']['info']['dmpid']=$idp;

					if(!$fid)
					{
						$fidp=$idp;
						$fid=$id;
					}

					$eltlist[$id]=$idp;
					$elter=$d['elt'];
					if(substr($elter,0,1) == 'N')
					{
						$elter=(int)substr($elter, 1);
						if($eltlist[$elter])
							$elter=$eltlist[$elter];
					}

					$dat=array
					(
						'mandanten_id'		=> $mand,
						'projekt_id'		=> (int)$idp,
						'mutterprojekt_id'	=> $GLOBALS['_cppj']['info']['dmpid'],
						'budget_id'			=> 0,
						'zeit_id'			=> (int)$idz,
						'elter'				=> (int)$elter,
						'projektart'		=> (int)$d['art'],
						'vorgangsnummer'	=> "'".$d['nvn']."'",
						'name_projekt'		=> "'".convert_string(substr($GLOBALS['_cppj']['info']['dpfx'].$d['npn'],0,$maxpn_len), 'todb')."'",
						'beschreibung'		=> $pdat[$id]['data']['beschreibung'],
						'kunde'				=> (int)$pdat[$id]['data']['kunde'],
						'ansprechpartner'	=> (int)$pdat[$id]['data']['ansprechpartner'],
						'projektmanager'	=> (int)$pdat[$id]['data']['projektmanager'],
						'projektleiter'		=> (int)$pdat[$id]['data']['projektleiter'],
						'vertreter'			=> (int)$pdat[$id]['data']['vertreter'],
						'kategorie_id'		=> (int)$pdat[$id]['data']['kategorie_id'],
						'projekt_status'	=> 297,	// geplant
						'prioritaet'		=> (int)$pdat[$id]['data']['prioritaet'],
						'verlauf'			=> $pdat[$id]['data']['verlauf'],
						'aufwand_soll'		=> get_double($pdat[$id]['aufwand_soll']),
						'aufwand_ist'		=> 0.00,
						'aufwand_todo'		=> 0.00,
						'max_zeit_aufw'		=> get_double($pdat[$id]['data']['max_zeit_aufw']),
						'deadline'			=> $pdat[$id]['data']['deadline'],
						'maart_id'			=> (int)$pdat[$id]['data']['maart_id'],
						'mafaehigkeit_id'	=> (int)$pdat[$id]['data']['mafaehigkeit_id'],
						'anmerkung'			=> $pdat[$id]['data']['anmerkung'],
						'anmerkungintern'	=> $pdat[$id]['data']['anmerkungintern'],
						'ref_nummer'		=> '',
						'wiederholungart'	=> '',
						'anzahl'			=> '',
						'erstelldatum'		=> "'".$jetzt."'"
					);

					if(strlen($dat['beschreibung']))
						$dat['beschreibung']	= "'".convert_string($dat['beschreibung'], 'todb')."'";
					if(strlen($dat['anmerkung']))
						$dat['anmerkung']		= "'".convert_string($dat['anmerkung'], 'todb')."'";
					if(strlen($dat['anmerkungintern']))
						$dat['anmerkungintern']	= "'".convert_string($dat['anmerkungintern'], 'todb')."'";
					if(strlen($dat['deadline']))
					{
						$dat['deadline']		= substr(time_add($dat['deadline'], $tad, 'D'), 0, 8).'240000';
						if($dat['deadline'] < $d['nsb'])
							$dat['deadline']	= substr($d['nsb'], 0, 8).'240000';
						if($dat['deadline'] > $d['nse'])
							$dat['deadline']	= $d['nse'];
						$dat['deadline']		= "'".$dat['deadline']."'";
					}
					if($dat['abhaengigkeit'] && $eltlist[$dat['abhaengigkeit']] && ($timeadjust != 1))
						$dat['abhaengigkeit']	= $eltlist[$dat['abhaengigkeit']];
					else
						$dat['abhaengigkeit']	= 0;

					if(fxIsArray($addinfo))
					{
						foreach($addinfo as $afield => $avalue)
						{
							if(($afield == 'kunde') || ($afield == 'ansprechpartner') || ($afield == 'projektmanager') || ($afield == 'projektleiter') || ($afield == 'vertreter') || ($afield == 'kategorie_id'))
							{
								$avalue=(int)$avalue;
								if($avalue >= 0)
									$dat[$afield]=$avalue;
							}
						}
					}

					get_cols_vals($dat);

					$sql="INSERT INTO projekte (".$GLOBALS['cols'].") VALUES (".$GLOBALS['vals'].")";
					$err=db_x($sql);
//fxDebug($err,$sql, 0);
					if($err)
						$err=1842; // Die Projektdaten eines Vorgangs konnten nicht angelegt werden! Der Kopierprozess wurde abgebrochen.

					// Auf Eindeutigkeit der Vorgangsnummer prüfen
					if(!$err)
					{
						$sql="SELECT projekt_id FROM projekte WHERE vorgangsnummer='".$d['nvn']."' AND projekt_id<>".$idp." AND mandanten_id=".$mand;
						$vid=(int)db_value($sql);
//fxDebug($vid,$sql, 0);
						if($vid)
							$err=1846; // Die erstellte Nummer eines Vorgangs ist nicht eindeutig! Der Kopierprozess wurde abgebrochen. Dieser Fehler kann auftreten falls in der Zwischenzeit eine andere Person einen Vorgang mit gleicher Hierarchie angelegt hat.
					}

					// INSERT: Tabelle "zeitdaten"
					if(!$err)
					{
						$dat=array
						(
							'mandanten_id'		=> $mand,
							'zuordnungs_mid'	=> $mand,
							'zeit_id'			=> $idz,
							'personen_id'		=> 0,
							'projekt_id'		=> $idp,
							'mutterprojekt_id'	=> $GLOBALS['_cppj']['info']['dmpid'],
							'soll_beginn'		=> "'".$d['nsb']."'",
							'soll_ende'			=> "'".$d['nse']."'",
							'zeitart'			=> 102
						);

						get_cols_vals($dat);

						$sql="INSERT INTO zeitdaten (".$GLOBALS['cols'].") VALUES (".$GLOBALS['vals'].")";
						$err=db_x($sql);
//fxDebug($err,$sql, 0);
						if($err)
							$err=1843; // Die Zeitdaten eines Vorgangs konnten nicht angelegt werden! Der Kopierprozess wurde abgebrochen.
					}

					// INSERT: Budgets
					if(!$err)
					{
						// Daten aufbereiten
						$barr=array
						(
							'budgettyp' => $pdat[$id]['data']['budgettyp'], 'abrechnungsart' => $pdat[$id]['data']['abrechnungsart'], 'abrechnungsart_mat' => $pdat[$id]['data']['abrechnungsart_mat'],
							'budgetrahmen_pers_int'	=> $pdat[$id]['data']['budgetrahmen_pers_int'],	'budgetrahmen_mat_int'	=> $pdat[$id]['data']['budgetrahmen_mat_int'],
							'budgetrahmen_pers_ext'	=> $pdat[$id]['data']['budgetrahmen_pers_ext'],	'budgetrahmen_mat_ext'	=> $pdat[$id]['data']['budgetrahmen_mat_ext'],
							'budget_pers_int_gepl'	=> $pdat[$id]['data']['budget_pers_int_gepl'],	'budget_mat_int_gepl'	=> $pdat[$id]['data']['budget_mat_int_gepl'],
							'budget_pers_ext_gepl'	=> $pdat[$id]['data']['budget_pers_ext_gepl'],	'budget_mat_ext_gepl'	=> $pdat[$id]['data']['budget_mat_ext_gepl']
						);
//fxDebug($barr, __LINE__.': '.$idp);
						$err_id=update_budget($idp, $barr);
						if($err_id === true)
							$err=1844; // Die Budgetdaten eines Vorgangs konnten nicht angelegt werden! Der Kopierprozess wurde abgebrochen.
						else if($err_id)
							$err=$err_id;
					}

					// PCA-Code
					if(!$err)
					{
						$npca=pca_code($idp);
						if($npca == '?')
							$err=817;	// Fehler beim Verwalten des PCA-Codes!
					}

					// Prefix
					if(!$err && $GLOBALS['_cppj']['info']['dpfxl'] && !isset($GLOBALS['mp_prefixes'][$GLOBALS['_cppj']['info']['dmpid']]))
					{
						$err=handle_prefix(FXP_CREATE, $GLOBALS['_cppj']['info']['dmpid'], $pdat[$id]['data']['kunde'], trim($GLOBALS['_cppj']['info']['dpfx']), false);
						if(!$err)
							getAllPrefixes();
					}

					// INSERT: Tabelle "maplanung" + "artikelzuordnung" - Ressourcen kopieren
					if(!$err && ($d['res'] || $d['sam']))
					{
						$sql  = "SELECT";
						$sql .= " m.personen_id, m.sachmittel_id, m.maart_id, m.mafaehigkeit_id, m.auslastung, m.aufwand_std, m.prozent,";
						$sql .= " a.artikel_id, a.artikeltyp, a.ek_projekt, a.vk_projekt";
						$sql .= " FROM maplanung m, artikelzuordnung a";
						$sql .= " WHERE m.projekt_id=".$id." AND m.maplanung_id=a.maplanung_id AND m.mandanten_id=".$mand." AND a.mandanten_id=".$mand;
						$rdt=db_values($sql, '*');
//fxDebug($rdt,$sql, 0);
						if(fxIsArray($rdt))
						{
							$nsb=substr($d['nsb'],0,8);
							$nse=substr($d['nse'],0,8);
							foreach($rdt as $rd)
							{
								// Prüfen, ob Person "inaktiv" oder nicht verfügbar im Aufgabenzeitraum
								$add=true;
								if(!$rd['sachmittel_id'])
								{
									if(!isset($pia[$rd['personen_id']]))
									{
										$sql="SELECT p.inaktiv,d.maeintrittsdatum,d.maaustrittsdatum FROM personen p LEFT JOIN madaten d ON (d.personen_id=p.personen_id AND d.mandanten_id=p.mandanten_id) WHERE p.personen_id=".$rd['personen_id']." AND p.mandanten_id=".$mand;
										$pda=db_values($sql);
										$pda['maeintrittsdatum']=substr($pda['maeintrittsdatum'],0,8);
										$pda['maaustrittsdatum']=substr($pda['maaustrittsdatum'],0,8);
										if(!strlen($pda['maaustrittsdatum']))
											$pda['maaustrittsdatum']='99999999';
										$pia[$rd['personen_id']]=$pda;
									}
//fxDebug($pia[$rd['personen_id']],'$pia['.$rd['personen_id'].'] -- $nsb='.$nsb.', $nse='.$nse);
									if($pia[$rd['personen_id']]['inaktiv'] || ($pia[$rd['personen_id']]['maeintrittsdatum'] > $nse) || ($pia[$rd['personen_id']]['maaustrittsdatum'] < $nsb))
										$add=false;
								}
//fxDebug($add,'$pia['.$rd['personen_id'].']');

								// Nur "aktive" Ressourcen übernehmen
								if($add)
								{
									$idmp=id_max('maplanung', 'maplanung_id');

									// Tabelle: "maplanung"
									$dat=array
									(
										'mandanten_id'		=> $mand,
										'maplanung_id'		=> $idmp,
										'projekt_id'		=> $idp,
										'personen_id'		=> $rd['personen_id'],
										'sachmittel_id'		=> $rd['sachmittel_id'],
										'maart_id'			=> $rd['maart_id'],
										'mafaehigkeit_id'	=> $rd['mafaehigkeit_id'],
										'soll_beginn'		=> "'".$d['nsb']."'",
										'soll_ende'			=> "'".$d['nse']."'",
										'auslastung'		=> 0,
										'aufwand_std'		=> 0,
										'prozent'			=> $rd['prozent'],
										'e_mail'			=> 0,
										'benutzergr_id'		=> 0
									);

									if(!$dat['prozent'])
										$dat['prozent']='';
									if($pdat[$id]['aufwand_soll'] && $dat['prozent'])
										$dat['aufwand_std']	= $pdat[$id]['aufwand_soll']*$dat['prozent'];

									// Sachmittel
									if($rd['sachmittel_id'])
									{
										$dat['personen_id']	= '';
										$dat['auslastung']	= 1;
									}
									// Person
									else
									{
										$dat['sachmittel_id']	= '';
										if($dat['aufwand_std'])
											$dat['auslastung']=auslastung($idp, $dat['personen_id'], $dat['aufwand_std']);
									}

									get_cols_vals($dat);

									$sql="INSERT INTO maplanung (".$GLOBALS['cols'].") VALUES (".$GLOBALS['vals'].")";
									$err=db_x($sql);
//fxDebug($err,$sql, 0);
									if($err)
									{
										$err=1847; // Die Ressourcendaten eines Vorgangs konnten nicht angelegt werden! Der Kopierprozess wurde abgebrochen.
										break;
									}

									// Tabelle: "artikelzuordnung"
									if(!$err)
									{
										$idaz=id_max('artikelzuordnung', 'artikelzuord_id');

										$dat=array
										(
											'mandanten_id'		=> $mand,
											'artikelzuord_id'	=> $idaz,
											'maplanung_id'		=> $idmp,
											'artikel_id'		=> $rd['artikel_id'],
											'artikeltyp'		=> $rd['artikeltyp'],
											'projekt_id'		=> $idp,
											'ek_projekt'		=> $rd['ek_projekt'],
											'vk_projekt'		=> $rd['vk_projekt'],
											'soll_beginn'		=> "'".$d['nsb']."'",
											'soll_ende'			=> "'".$d['nse']."'"
										);

										get_cols_vals($dat);

										$sql="INSERT INTO artikelzuordnung (".$GLOBALS['cols'].") VALUES (".$GLOBALS['vals'].")";
										$err=db_x($sql);
//fxDebug($err,$sql, 0);
										if($err)
										{
											$err=1848; // Die Artikeldaten eines Vorgangs konnten nicht angelegt werden! Der Kopierprozess wurde abgebrochen.
											break;
										}
									}
								}
							}
						}
					}

					// INSERT: Tabelle "kostenzuordnung"
					if(!$err)
					{
						$sql  = "SELECT";
						$sql .= " kostenstelle_id, kostenstellenart, kostendefinition, gueltig_von, gueltig_bis";
						$sql .= " FROM kostenzuordnung";
						$sql .= " WHERE mandanten_id=".$mand." AND id=".$id." AND kostenstellenart IN (0,904,905,906,907) AND (historie=0 OR historie IS NULL)";
						$kst=db_values($sql, '*');
//fxDebug($kst,$sql, 0);
						if(fxIsArray($kst))
						{
							foreach($kst as $k)
							{
								$kgv=trim($k['gueltig_von']);
								if(strlen($kgv))
									$kgv="'".$kgv."'";
								else
									$kgv='NULL';

								$kgb=trim($k['gueltig_bis']);
								if(strlen($kgb))
									$kgb="'".$kgb."'";
								else
									$kgb='NULL';

								$dat=array
								(
									'mandanten_id'		=> $mand,
									'kostenstelle_id'	=> $k['kostenstelle_id'],
									'id'				=> $idp,
									'kostenstellenart'	=> $k['kostenstellenart'],
									'kostendefinition'	=> $k['kostendefinition'],
									'gueltig_von'		=> $kgv,
									'gueltig_bis'		=> $kgb,
									'historie'			=> 'NULL'
								);

								get_cols_vals($dat);

								$sql="INSERT INTO kostenzuordnung (".$GLOBALS['cols'].") VALUES (".$GLOBALS['vals'].")";
								$err=db_x($sql);
//fxDebug($err, $sql);
								if($err)
								{
									$err=2900; // Die Kostenstellendaten eines Projektes konnten nicht angelegt werden! Der Kopierprozess wurde abgebrochen.
									break;
								}
							}
						}
					}
				}

				if($err)
					break;
			}
//$err=true;

			// Projektfortschritts-Definiton
			if(!$err && $fidp)
			{
				$vMT=1784;
				$vZP=1790;
				$vTG=7;
				if(fxIsArray($addinfo))
				{
					foreach($addinfo as $afield => $avalue)
					{
						if(($afield == 'methode') || ($afield == 'zeitpunkt') || ($afield == 'tage'))
						{
							$avalue=(int)$avalue;
							if($avalue >= 0)
							{
								if($afield == 'methode')
									$vMT=$avalue;
								else if($afield == 'zeitpunkt')
									$vZP=$avalue;
								else
									$vTG=$avalue;
							}
						}
					}
				}
				$err=projektfortschritt($fidp, 0, 0.00, $vMT, $vZP, $vTG);
//echo('$err=projektfortschritt('.$fidp.', 0, 0.00, '.$vMT.', '.$vZP.', '.$vTG.');<BR>');
			}

			// Aufwandsanpassung
			if(!$err && $fid && $prelist)
			{
				// Budgetinfos
				$sql  = "SELECT projekt_id, aufwand_soll";
				$sql .= " FROM projekte p";
				$sql .= " WHERE projekt_id IN (".$prelist.") AND mandanten_id=".$mand;
				$bin=db_values($sql, '*');
				if(fxIsArray($bin))
				{
					foreach($bin as $b)
					{
						// Aufwandsdaten anpassen
						$aws=(double)$b['aufwand_soll']+(double)$pdat[$fid]['aufwand_soll'];

						$sql="UPDATE projekte SET aufwand_soll=".$aws." WHERE projekt_id=".$b['projekt_id']." AND mandanten_id=".$mand;
						$err=db_x($sql);
//fxDebug($err, $sql);
						if($err)
						{
							$err=1851; // Die Projektdaten eines Vorgangs konnten nicht angepasst werden! Der Kopierprozess wurde abgebrochen.
							break;
						}
					}
				}
			}

			// Maplanung + Auslastung anpassen
			if(!$err && $GLOBALS['_cppj']['info']['dmpid'] && $res)
			{
				$err=update_maplanung('person', $mand, $GLOBALS['_cppj']['info']['dmpid']);
				if(!$err)
					$err=update_maplanung('sachmittel', $mand, $GLOBALS['_cppj']['info']['dmpid']);
				if($err)
					$err=1856;	// Die Ressourcenplanung konnte nicht angepasst werden! Der Kopierprozess wurde abgebrochen.
			}
		}
		else
			return 1839;	// Es wurden keine Daten zum verarbeiten gefunden?
	}

	// Rückgabe der Fehlernummer im Fehlerfall, sonst false
	return $err;
}

/**
 * ???FUNCTION???
 *
 * @param various $dat - Mandatory parameter: ???PARAMETER???
 */
function get_cols_vals($dat)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$GLOBALS['cols']='';
	$GLOBALS['vals']='';
	if(fxIsArray($dat))
	{
		foreach($dat as $col => $val)
		{
			if($GLOBALS['cols'])
				$GLOBALS['cols'] .= ', ';
			$GLOBALS['cols'] .= $col;
			if($GLOBALS['vals'])
				$GLOBALS['vals'] .= ', ';
			if(strlen($val))
				$GLOBALS['vals'] .= $val;
			else
				$GLOBALS['vals'] .= 'NULL';
		}
	}
}
?>