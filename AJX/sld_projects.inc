<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : prj_menu.inc                                                 //
// Version     : 21.2                                                         //
// Begin       : 2020-10-29                                                   //
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
 * Slider: "PROJECTS"
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

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

$save_globals=false;

// GET parameters
// ...Open Project Number
$opa=false;
$opn='';
$opi=0;
if(fxIsArray($_GET) && isset($_GET['opn']) && strlen($_GET['opn']))
{
	$opn=$_GET['opn'];
	if(substr($opn,0,1) == '!')
	{
		$opa=true;
		$opn=substr($opn,1);
	}
	if(strlen($opn))
	{
		$sql="SELECT projekt_id FROM projekte WHERE vorgangsnummer='".$opn."' AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$opi=(int)db_value($sql);
//fxDebug($opi,$sql);
		if(!$opi)
			$opn='';
	}
};
$opl=strlen($opn);
$all=true;
if($opl)
	$all=false;
// ...Mark Project Number
$mpn='';
$mpi=0;
if(fxIsArray($_GET) && isset($_GET['mpn']) && strlen($_GET['mpn']))
{
	$mpn=$_GET['mpn'];
	$sql="SELECT projekt_id FROM projekte WHERE vorgangsnummer='".$mpn."' AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$mpi=(int)db_value($sql);
//fxDebug($mpi,$sql);
	if(!$mpi)
		$mpn='';
	fxSession('g','p','project',$mpi);
	fxSession('g','s');
	die;
}
else if($GLOBALS['fxpglobals']['project'])
{
	$sql="SELECT vorgangsnummer FROM projekte WHERE projekt_id='".$GLOBALS['fxpglobals']['project']."' AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$mpn=db_value($sql);
//fxDebug($mpn,$sql);
	if(strlen($mpn))
		$mpi=$GLOBALS['fxpglobals']['project'];
	else
	{
		fxSession('g','p','project',0);
		$save_globals=true;
	}
}
// ...Sort Order
if(fxIsArray($_GET) && isset($_GET['srt']) && strlen($_GET['srt']))
{
	// Save setting in db
	$sval=1465;
	if($_GET['srt'] == 'j')
		$sval=1464;

	$whr=" WHERE einstellungs_id=109 AND benutzer_id=".$GLOBALS['fxpglobals']['user']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$chk="SELECT einstellungswerte_id FROM s_einstellungswerte".$whr;
	$cid=(int)db_value($chk);
	if($cid)
		$eid=$cid;
	else
		$eid=id_max('s_einstellungswerte', 'einstellungswerte_id', '', '', 401);

	if(!$cid)	// INSERT
	{
		$sql  = "INSERT INTO s_einstellungswerte";
		$sql .= " (mandanten_id, benutzer_id, einstellungswerte_id, einstellungs_id, einstellungswert) ";
		$sql .= "VALUES";
		$sql .= " (".$GLOBALS['fxpglobals']['client'].", ".$GLOBALS['fxpglobals']['user'].", ".$eid.", 109, '".$sval."')";
	}
	else		// UPDATE
	{
		$sql  = "UPDATE s_einstellungswerte";
		$sql .= " SET einstellungswert='".$sval."'";
		$sql .= $whr;
	}
	$eerr=db_x($sql);
//fxDebug($eerr, $sql);

	$GLOBALS['fxpglobals']['settings']['vmn_projektnr']=meldung($sval,true);
	$save_globals=true;
}
$srt=strtolower(substr($GLOBALS['fxpglobals']['settings']['vmn_projektnr'],0,1));

// POST parameters
$pfo=array();
if(fxIsArray($_POST) && isset($_POST['pfo']) && strlen($_POST['pfo']))
{
	$tmp=explode('|',$_POST['pfo']);
	foreach($tmp as $t)
		$pfo[$t]=true;
}
// ...project search
$src='';
if(fxIsArray($_POST) && isset($_POST['src']) && strlen($_POST['src']))
{
	$src=trim($_POST['src']);
	$srl=trim(strtolower($src));
	if(($srl == 'suche') || ($srl == 'search'))
		$src='';
}
$osrc=false;
if($src != $GLOBALS['fxpglobals']['psearch'])
{
	fxSession('g','p','psearch',$src);
	$save_globals=true;
	if(strlen($src))
		$osrc=true;
}
// ...compare string
$pcs='';
if(fxIsArray($_POST) && isset($_POST['pcs']) && strlen($_POST['pcs']))
	$pcs=trim($_POST['pcs']);

//fxDebug($pfo,'$pfo: '.$_POST['pfo']);
//fxDebug($src,'$src: '.$_POST['src']);

// Get last change dates
// ...projects
$lcdp=db_value("SELECT MAX(zeitstempel) AS mz FROM projekte WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']);
// ...times
$lcdt=db_value("SELECT MAX(zeitstempel) AS mz FROM zeitdaten WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']);
// ...responsibility
$lcdy=db_value("SELECT MAX(zeitstempel) AS mz FROM projekte_parameter WHERE parameter_txt='PV' AND parameter_int=".$GLOBALS['fxpglobals']['person']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
// ...resource planning
$lcdr=db_value("SELECT MAX(zeitstempel) AS mz FROM maplanung WHERE personen_id=".$GLOBALS['fxpglobals']['person']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
// ...access rights
$lcda=db_value("SELECT MAX(zeitstempel) AS mz FROM rechte_referenz WHERE rechte_ref=37 AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
// ...skills
$lcds=db_value("SELECT MAX(zeitstempel) AS mz FROM ma_art_faehigkeiten WHERE personen_id=".$GLOBALS['fxpglobals']['person']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
// ...filter settings
$lcdf=db_value("SELECT MAX(zeitstempel) AS mz FROM teinstellung WHERE benutzer_id=".$GLOBALS['fxpglobals']['user']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
$lcgf=$GLOBALS['fxpglobals']['setfilter'];
$cs=$lcdp.'|'.$lcdt.'|'.$lcdy.'|'.$lcdr.'|'.$lcda.'|'.$lcds.'|'.$lcdf.'|'.$lcgf.'|'.$srt.'|'.$src;
//fxDebug($cs,'$cs');
//fxDebug($pcs,'$pcs');

$pm='';
if(($cs !== $pcs) || $opl || $opa)
{
	if($all)
		$pm .= '<div id="pjsmd" style="position:absolute;line-height:20px;">'.$GLOBALS['nl'];

	// Literals
	if($GLOBALS['fxpglobals']['lang'] == 1)
		$la=array('id'=>"ID", 'st'=>"Status", 'pr'=>"Priorit&auml;t", 'so'=>"Soll-Daten", 'is'=>"Ist-Daten", 'be'=>"Beschreibung", 'nb'=>"Noch nicht begonnen", 'nup'=>"Anzahl nicht berechtigter Projekte/Aufgaben", 'nap'=>"Anzahl berechtigter Projekte/Aufgaben", 'tnp'=>"Gesamtanzahl Projekte/Aufgaben", 'nfp'=>"Anzahl gefilterter (= nicht angezeigter) Projekte/Aufgaben");
	else
		$la=array('id'=>"ID", 'st'=>"Status", 'pr'=>"Priority", 'so'=>"Planned Data", 'is'=>"Actual Data", 'be'=>"Description", 'nb'=>"Not yet started", 'nup'=>"Number of unauthorized projects/tasks", 'nap'=>"Number of authorized projects/tasks", 'tnp'=>"Total number of projects/tasks", 'nfp'=>"Number of filtered (= undisplayed) projects/tasks");

	// Replacement arrays
	$ta=array('<'=>"&lt;", '>'=>"&gt;", '"'=>"&quot;");
	$ra=array('<'=>"&lt;", '>'=>"&gt;", '"'=>"&quot;", '(*copy)'=>"<font class=\"s2 normal darkgrey\">(*copy)</font>");

	// Projct types
	$ptypes=FXP_PRJ_MAIN.",".FXP_PRJ_SUB.",".FXP_PRJ_TASK;

	// Get amount of all project
	$sql="SELECT COUNT(projekt_id) AS pc FROM projekte WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND projektart IN (".$ptypes.")";
	$pno_t=(int)db_value($sql);
	$pno_r=0;
	$pno_d=0;

	// Projects found?
	if($pno_t > 0)
	{
		// Get user rights on projects
		$pra=false;
		$prs='';
		if(!$GLOBALS['fxpglobals']['person'] || ($GLOBALS['fxpglobals']['user'] == 1) || $GLOBALS['fxpglobals']['persdat']['roles'][2])
		{
			$pno_r=$pno_t;
			$pra=true;
		}
		else
		{
			$prs=prights_get(0);
//fxDebug($prs,'$prs');
			if($prs !== "-666")
			{
				$sql="SELECT projekt_id, vorgangsnummer FROM projekte WHERE projekt_id IN (".$prs.") AND projektart IN (".$ptypes.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
				if(fxIsArray($tmp))
				{
					$tpa=array();
					$pra=array();
					foreach($tmp as $t)
					{
						$pn=$t['vorgangsnummer'];
						$pra[$pn]=(int)$t['projekt_id'];
						while(strlen($pn) && !isset($tpa[$pn]))
						{
							$tpa[$pn]=true;
							if(!isset($pra[$pn]))
								$pra[$pn]=0;
							$pno_r++;
							$pn=substr($pn,0,-5);
						}
					}
				}
			}
		}
//fxDebug($pra,'$pra');

		// User has project rights?
		if($pno_r > 0)
		{
			// Lookup values
			$lpsa=fxf_getReferenceValues('projekt_status');
			$lpra=fxf_getReferenceValues('prioritaet');

			// Filter
			$pfa=array();
			$paa=array();
			$sql  = "SELECT p.projekt_id, p.vorgangsnummer";
			$sql .= " FROM projekte p, zeitdaten z";
			$sql .= " WHERE z.projekt_id=p.projekt_id AND z.zeitart=102 AND z.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
			if(strlen($prs))
				$sql .= " AND p.projekt_id IN (".$prs.")";
			$flt=filter_projekt('and',false,false);
//fxDebug($flt,'$flt');
			if(strlen($flt))
				$sql .= " ".$flt;
			if(strlen($src))
			{
				$vns='';
				if(is_numeric(substr($src,0,1)))
				{
					$se=explode('.',$src);
					if(sizeof($se) > 1)
					{
						foreach($se as $svn)
						{
							if(strlen($vns))
								$vns .= '.';
							if(strlen($svn))
								$vns .= substr('0000'.(int)$svn,-4);
						}
					}
					else
						$vns=(int)$src;
				}
				$sql .= " AND ((p.projekt_id=".(int)$src.") OR (".db_like('p.name_projekt', '%'.$src.'%').") OR (".db_like('p.ref_nummer', '%'.$src.'%').")";
				if(strlen($vns))
					$sql .= " OR (p.vorgangsnummer LIKE '%".$vns."%')";
 				$sql .= ")";
			}
			$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
			$fids="";
			if(fxIsArray($tmp))
			{
				$tpa=array();
				foreach($tmp as $t)
				{
					$pn=$t['vorgangsnummer'];
					$pfa[$pn]=(int)$t['projekt_id'];
					$paa[$pn]=true;
					$fids .= ", ".$pfa[$pn];
					while(strlen($pn) && !isset($tpa[$pn]))
					{
						$tpa[$pn]=true;
						if(!isset($pfa[$pn]))
						{
							$paa[$pn]=false;
							if(fxIsArray($pra) && isset($pra[$pn]))	// User has rights
								$pfa[$pn]=(int)$pra[$pn];
							else if($pra === true)					// User has all rights
								$pfa[$pn]=-666;
							else
								$pfa[$pn]=0;
						}
						$pn=substr($pn,0,-5);
					}
				}
			}
			if(strlen($fids))
				$fids=substr($fids,2);
//fxDebug($pfa,'$pfa');
//fxDebug($paa,'$paa');
//fxDebug($fids,'$fids');

			$pno_d=sizeof($pfa);

			// Get display project number array
			$pda=array();
			$pia=array();
			if(fxIsArray($pfa))
			{
				ksort($pfa,SORT_STRING);
//fxDebug($pfo,'$pfo: '.$_POST['pfo']);
				$onl=$opl+5;
				foreach($pfa as $pn => $pid)
				{
					if(isset($pfo[$pn]) || ($opa && !$opl) || $osrc)	// Previous opened project or Open all or Search
					{
						$pda[$pn]=$pid;
						$pia[$pn]=true;
					}
					else
					{
						if($opl && (substr($pn,0,$opl) == $opn) && (($pn == $opn) || (strlen($pn) == $onl) || $opa))	// Just opened project(s)
						{
							$pda[$pn]=$pid;
							if($opa || ($pn == $opn))
								$pia[$pn]=true;
						}
						else if(strlen($pn) == 4)	// Main project
							$pda[$pn]=$pid;
						else
						{
							$en=substr($pn,0,-5);
							if(isset($pfo[$en]))
								$pda[$pn]=$pid;
						}
					}
				}
			}
//fxDebug($pia,'$pia');
			$dids="";
			$dpns="";
			foreach($pda as $pno => $pid)
			{
				if($pid && ($pid != -666))
					$dids .= ", ".$pid;
				else
					$dpns .= " OR vorgangsnummer='".$pno."'";
			}
			if(strlen($dids))
				$dids=substr($dids,2);
			if(strlen($dpns))
				$dpns=substr($dpns,4);
//fxDebug($pda,'$pda');
//fxDebug($dids,'$dids');
//fxDebug($dpns,'$dpns');

			// Read all projects
			$par=array();
			$poa=array();
			if(strlen($dids) || strlen($dpns))
			{
				$sql  = "SELECT";
				$sql .= " p.vorgangsnummer, p.projekt_id, p.mutterprojekt_id, p.projektart, p.name_projekt, p.prioritaet, p.projekt_status, p.aufwand_soll, p.aufwand_ist, p.ref_nummer, p.beschreibung,";
				$sql .= " z.soll_beginn, z.soll_ende, z.ist_beginn, z.ist_ende ";
				$sql .= "FROM";
				$sql .= " projekte p, zeitdaten z ";
				$sql .= "WHERE";
				$sql .= " z.zeit_id=p.zeit_id AND z.mandanten_id=p.mandanten_id AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND p.projektart IN (".$ptypes.")";
				$sql .= " AND (";
				if(strlen($dids))
				{
					$sql .= "p.projekt_id IN (".$dids.")";
					if(strlen($dpns))
						$sql .= " OR ";
				}
				if(strlen($dpns))
					$sql .= "p.projekt_id IN (SELECT projekt_id FROM projekte WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND (".$dpns."))";
				$sql .= ") ";
				$sql .= "ORDER BY";
				$sql .= " p.vorgangsnummer";
				$par  = db_values($sql,'*');
//fxDebug($par,$sql);

				if(fxIsArray($par))
				{
					// Sort order
					$sort_by_number=true;
					if($srt == 'n')
						$sort_by_number=false;
//fxDebug($sort_by_number,'$sort_by_number');

					$mpa=array();
					foreach($par as $pc => $p)
					{
						$pn=$p['vorgangsnummer'];
						if((strlen($pn) == 4) && ($p['projektart'] == FXP_PRJ_MAIN))
						{
							if($sort_by_number)
								$k=$pn;
							else
								$k=str_replace(' ','-',strtolower($p['name_projekt'].'-'.$p['ref_nummer'])).'-'.$pn;
							$mpa[$k]=$pc;
						}
					}
					if(sizeof($mpa))
						ksort($mpa,SORT_STRING);
//fxDebug($mpa,'$mpa');

					foreach($mpa as $mk => $mc)
					{
						$poa[]=$mc;
						$mn=$par[$mc]['vorgangsnummer'];
						foreach($par as $pc => $p)
						{
							$pn=$p['vorgangsnummer'];
							if((strlen($pn) > 4) && (substr($pn,0,4) == $mn))
								$poa[]=$pc;
						}
					}
//fxDebug($poa,'$poa');
				}
			}

			if(sizeof($par) && sizeof($poa))
			{
				// Prefix settings
				$disp_prefix=true;
				if(strtolower(substr($GLOBALS['fxpglobals']['settings']['zeige_prefix'],0,1)) == 'n')
					$disp_prefix=false;
				getAllPrefixes();
//fxDebug($GLOBALS['mp_prefixes'],'mp_prefixes');

				$mppn='';
				foreach($poa as $k => $pc)
				{
					$p=$par[$pc];

					$id=(int)$p['projekt_id'];
					$mi=(int)$p['mutterprojekt_id'];
					$pn=$p['vorgangsnummer'];

					$pl=floor(strlen($par[$pc-1]['vorgangsnummer'])/5);
					$lv=floor(strlen($pn)/5);

					// Spacer line
					if(!$lv && strlen($mppn))
					{
						$pd='none';
						if(isset($pia[$mppn]))
							$pd='';
						if($all)
							$pm .= '<div id="pjsms_'.$mppn.'" style="height:6px;display:'.$pd.';"></div>'.$GLOBALS['nl'];
					}

					// Rights: 0=Display only, 1=OK
					$pr=0;
					if($pfa[$pn])
						$pr=2;

					// Structure lines
					$li='';
					$lp=0;
					if($lv > 0)
					{
						for($l=1; $l<$lv; $l++)
						{
							// Search if a process for the same branch exists below with same level
							$pf=false;
							if(fxIsArray($pda))
							{
								$amn=substr($pn,0,4);
								$ppn=substr($pn,0,$l*5);	// lv=1: 0001.0001 => 0001.
								$ppl=strlen($ppn);
								foreach($pda as $pfn => $pfid)
								{
									$pmn=substr($pfn,0,4);
									if($pmn > $amn)
										break;
									else if($pmn == $amn)
									{
										$pfl=floor(strlen($pfn)/5);
										if(($pfn > $pn) && (substr($pfn,0,$ppl) == $ppn) && ($pfl == $l))
										{
											$pf=true;
											break;
										}
									}
								}
							}
							$li .= '<div style="position:absolute;left:'.$lp.'px;top:-14px;width:14px;">';
							if($pf)
								$li .= '<img src="'.$GLOBALS['gfxpath'].'lll.png" align="top" style="padding-top:6px;">';
							else
								$li .= '&nbsp;';
							$li .= '</div>';
							$lp += 14;
						}
						$ic='lsl';
						if($pl < $lv)
							$ic='lss';
						$li .= '<div style="position:absolute;left:'.$lp.'px;top:-14px;width:14px;"><img src="'.$GLOBALS['gfxpath'].$ic.'.png" align="top" style="padding-top:6px;"></div>';
						$lp += 14;
					}

					// Folder icon
					$idd='';
					$idn='';
					$isd='z-index:9999;';
					$isi='z-index:9999;';
					if($p['projektart'] < FXP_PRJ_TASK)
					{
						$ic='fc';

						// Check, if there is a next level project
						$nlp=false;
						if(fxIsArray($pfa))
						{
							$pnl1=strlen($pn)+1;
							foreach($pfa as $pfn => $pfid)
							{
								if(substr($pfn,0,$pnl1) === $pn.'.')
								{
									$nlp=true;
									break;
								}
							}
						}

						if($nlp)
						{
							if(isset($pia[$pn]))
								$ic='fo';

							$idd .= ' id="pjsmfd_'.$pn.'"';
							$idn .= ' id="pjsmfn_'.$pn.'"';
							$isd .= 'cursor:pointer;';
							$isi .= 'cursor:pointer;';
						}
						else
						{
							$isd .= 'cursor:help;';
							$isi .= 'cursor:default;opacity:0.2;';
						}
					}
					else
					{
						$isd .= 'cursor:help;';
						$isi .= 'cursor:default;';
						$ic='lt'.$p['projekt_status'];
					}
					$li .= '<div'.$idd.' style="position:absolute;left:'.$lp.'px;top:0;width:14px;'.$isd.'"><img id="pjsmfi_'.$pn.'" src="'.$GLOBALS['gfxpath'].$ic.'.png" align="top" style="padding-top:6px;'.$isi.'"></div>';
					$lp += 14;

					$style='';
					if($pr)
					{
						$style .= 'cursor:pointer;';
						if(!$paa[$pn])
							$style .= 'opacity:0.2;';
					}
					else
						$style .= 'cursor:help;opacity:0.2;';
					if(strlen($mpn) && ($pn == $mpn))	// Mark project
						$style .= 'border-radius:4px;box-shadow:0 0 4px #006b9f;padding:0 4px;';

					// Tooltip
					$tt  = "<b class=black>".text_wrap(strtr($p['name_projekt'],$ta), true, 80)."</b><div class=rbt></div>";
					$tt .= "<table border=0 cellpadding=2 cellspacing=0>";
					$tt .= "<tr><td class=grey>".$la['id'].":&nbsp;</td><td colspan=2>".$p['projekt_id']."</td>";
					$tt .= "<tr><td class=grey>".$la['st'].":&nbsp;</td><td colspan=2>".$lpsa[$p['projekt_status']]."</td>";
					$tt .= "<tr><td class=grey>".$la['pr'].":&nbsp;</td><td colspan=2>".$lpra[$p['prioritaet']]."</td>";
					$ds=get_entry($p['soll_beginn'],'datum');
					$de=get_entry($p['soll_ende'],'datum');
					$d=$ds;
					if($de != $ds)
						$d .= '-'.$de;
					$tt .= "<tr><td class=grey>".$la['so'].":&nbsp;</td><td>".$d."</td><td align=right>&nbsp;".get_entry($p['aufwand_soll']/3600,'dezimal')."&nbsp;h</td>";
					if(strlen($p['ist_beginn']))
					{
						$ds=get_entry($p['ist_beginn'],'datum');
						$d=$ds;
						if(strlen($p['ist_ende']))
						{
							$de=get_entry($p['ist_ende'],'datum');
							if($de != $ds)
								$d .= '-'.$de;
						}
						else
							$d .= '-?';
					}
					else
						$d="<i class=lightergrey>(".$la['nb'].")</i>";
					if($pr)
					{
						$tt .= "<tr><td class=grey>".$la['is'].":&nbsp;</td><td>".$d."</td><td align=right>&nbsp;".get_entry($p['aufwand_ist']/3600,'dezimal')."&nbsp;h</td>";
						if(strlen($p['beschreibung']))
							$tt .= "<tr><td class=grey valign=top>".$la['be'].":&nbsp;</td><td colspan=2>".text_wrap(strtr($p['beschreibung'],$ta), true, 48)."</td>";
					}
					$tt .= "</table>";

					// Project number
					$vn=no_null($pn);

					// Reference no.
					$rn='';
					if(strlen($p['ref_nummer']))
						$rn=strtr($p['ref_nummer'],$ra);

					// Prefix + Project name
					$pp='';
					$np=strtr($p['name_projekt'],$ra);
					if(sizeof($GLOBALS['mp_prefixes']) && isset($GLOBALS['mp_prefixes'][$mi]))
					{
						$plen=strlen($GLOBALS['mp_prefixes'][$mi]);
						if(substr($np,0,$plen) == $GLOBALS['mp_prefixes'][$mi])
						{
							$np=trim(substr($np,$plen));
							if($disp_prefix || !$lv)
								$pp=$GLOBALS['mp_prefixes'][$mi];
						}
					}

					// Pattern replace
					if(strlen($src))
					{
						$pattern='/'.preg_quote(strtolower($src), '/').'/i';

						$vn=preg_replace($pattern,"<b".$idn." class=\"red\">$0</b>",$vn);
						if(strlen($p['ref_nummer']))
							$rn=preg_replace($pattern,"<b class=\"red\">$0</b>",$rn);
						$np=preg_replace($pattern,"<b class=\"red\">$0</b>",$np);
					}

					if(strlen($p['ref_nummer']))
					{
						$rnid='';
						if($pr)
							$rnid=' id="pjsmp_'.$pn.'"';
						$rn='<i'.$rnid.' class="s2 normal darkgrey">('.$rn.')</i>&nbsp;';
					}

					// Number
					if($p['projekt_status'] == FXP_PS_INACTIVE)
						$cl='lightergrey';
					else if($p['projekt_status'] == FXP_PS_COMPLETED)
						$cl='darkred';
					else if($p['projekt_status'] == FXP_PS_ACTIVE)
						$cl='darkgrey';
					else
						$cl='grey';
					$no  = '<span'.$idn.' class="'.$cl.'" style="'.$isd.'" tooltip="'.$tt.'">';
					if($sort_by_number || $lv)
						$no .= $vn.'&nbsp;&nbsp;';
					else
						$no .= '&nbsp;&nbsp;'.$vn;
					$no .= '</span>';

					// Name
					$nm = '<span';
					if($pr)
						$nm .= ' id="pjsmp_'.$pn.'"';
					if($p['projektart'] == FXP_PRJ_MAIN)
					{
						$cl='bold italic';
						// Funded project
						if(strlen($p['ref_nummer']) && (substr($p['ref_nummer'],0,2) == 'EP'))
							$cl .= ' blue';
					}
					else if($p['projektart'] < FXP_PRJ_TASK)
						$cl='darkgrey italic';
					else
						$cl='normal';
					if(strlen($pp))
						$pp='<span class="darkgrey">'.$pp.'</span>';
					$nm .= ' class="'.$cl.'" style="'.$style.'" tooltip="'.$tt.'">';
					$nm .= $rn.$pp.$np;
					$nm .= '</span>';
					if($all || ($opl && (substr($pn,0,$opl) == $opn)))
					{
						$pm .= '<div id="pjsmd_'.$pn.'" style="position:relative;white-space:nowrap;display:block;">'.$li;
						$pm .= '<div style="position:relative;left:'.$lp.'px;top:0;">';
						if($sort_by_number || $lv)
							$pm .= $no.$nm;
						else
							$pm .= $nm.$no;
						$pm .= '</div>';
						$pm .= '</div>';
					}

					if(!$lv)
						$mppn=$pn;
				}
			}
			else if($all)
			{
				// 1481: Es wurden keine Projekte gefunden, die den eingestellten Filterkriterien entsprechen.
				$msg=meldung(1481);
					$pm .= '<i class="grey">'.$msg.'</i><br>'.$GLOBALS['nl'];
			}
		}
		else if($all)
		{
			// 1720: Ihnen wurden noch keine Rechte auf irgendein Projekt zugeteilt!
			$msg=meldung(1720);
			$pm .= '<i class="grey">'.$msg.'</i><br>'.$GLOBALS['nl'];
		}
	}
	else if($all)
	{
		// 1719: Es wurden für diesen Mandanten noch keine Projekte angelegt!
		$msg=meldung(1719);
		$pm .= '<i class="grey">'.$msg.'</i><br>'.$GLOBALS['nl'];
	}

	if($all)
	{
		$pm .= '<br>'.$GLOBALS['nl'];
		$pm .= '</div>';

		$pm .= '<input id="pjsmm" type="hidden" value="'.$mpn.'">';

		$pm .= '<input id="pjscs" type="hidden" value="'.$cs.'">';

		// Stats
		$pm .= '<div id="pjsmi" style="display:none;"><img src="'.$GLOBALS['gfxpath'].'tl_';
		if($pno_r)
			$pm .= 'green';
		else
			$pm .= 'red';
		$pm .= '.png" style="cursor:help;" tooltip="'.$la['tnp'].'">&nbsp;';
		$pm .= '<span class="black">'.$pno_r.'</span>';
		$pno_c=$pno_d;
		if($pno_c > $pno_r)
			$pno_c=$pno_r;
		if($pno_c < $pno_r)
			$pm .= '&nbsp;&nbsp;<span class="s4 lightergrey">&rarr;</span>&nbsp;&nbsp;<img src="'.$GLOBALS['gfxpath'].'flt0.png" style="cursor:help;" tooltip="'.$la['nfp'].'">&nbsp;<span class="black">'.($pno_r-$pno_c).'</span>';
		$pm .= '&nbsp;&nbsp;<span class="s4 lightergrey">&rarr;</span>&nbsp;&nbsp;<b class="black">'.$pno_c.'</b></div>';
	}
}

if($save_globals)
	fxSession('g','s');

die($pm);
?>