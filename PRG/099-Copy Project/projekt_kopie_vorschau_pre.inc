<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : projekt_kopie_vorschau_pre.inc                               //
// Version     : 21.2                                                         //
// Begin       : 2020-12-10                                                   //
// Last Change : 2020-12-10                                                   //
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
 * 99: Copy Project
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$restore_inputs=false;
if(fxIsArray($_POST) && fxIsArray($GLOBALS['_cppj']) && isset($_POST['Button_Submit']))
{
	if(fxIsArray($_POST['Name_Projekt']) && ($GLOBALS['_cppj']['info']['sid'] == $src_id) && ($GLOBALS['_cppj']['info']['did'] == $dst_id))
		$restore_inputs=true;
	unset($GLOBALS['_cppj']);
}

if(!$db_aktion)
	$cerr=copy_project($src_id, $dst_id, $inf, $res, $tad, $timeadjust, $GLOBALS['fxpglobals']['client']);

if(fxIsArray($GLOBALS['_cppj']) && fxIsArray($GLOBALS['_cppj']['data']))
{
//fxDebug($GLOBALS['_cppj']['info'],'info');

	// HINWEIS: Alle kopierten Abhängigkeiten der Quellvorgänge wurden aufgelöst!
	if($GLOBALS['_cppj']['info']['sdp'] && ($timeadjust == 1) && $_POST['Button_Timeadjust1'])
		error_msg(1859);

	$index=true;

	$Vorgangsnummer_nw=true;
	$Name_Projekt_nw=true;
	$Soll_Beginn_nw=true;
	$Soll_Ende_nw=true;

	$z=0;
	$mc=$GLOBALS['_maskcounter'];
	$te=0;
	$pr=0;
	$ps=0;

	$inakt=' {'.get_text(1391, '20').'}';	// {inaktiv}

	$fa=array('Checkbox|0', 'Vorgangsnummer', 'Name_Projekt', 'nv_text3|0', 'Soll_Beginn', 'Soll_Ende');
	fxf_maskColumnBorder(array('Checkbox', 'nv_text3'), 0, -1, $mc);

	// Prefix
	if($GLOBALS['_cppj']['info']['dpfxl'])
		$Name_Projekt_lit=get_text(46,'15').' <span class="normal">(Prefix: '.$GLOBALS['_cppj']['info']['dpfx'].')</span>';

	foreach($GLOBALS['_cppj']['data'] as $id => $d)
	{
		// (Checkbox)
		if(($d['md'] == 1) || ($d['md'] == 3))
			$cb='~|';
		else
		{
			if($restore_inputs || (!$_POST['Button_Select'] && !$_POST['Button_Submit'] && ($err || ($osrc_id == $src_id))))
				$cb=$_POST['Checkbox'][$mc][$z];
			else if($d['md'])
				$cb=1;
			else
				$cb='';

			if($cb)
				$GLOBALS['_cppj']['data'][$id]['md']=2;
			else
				$GLOBALS['_cppj']['data'][$id]['md']=0;
		}
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['checkbox']=$cb;

		// Vorg. Nr. (Vorgangsnummer)
		$lv=substr_count($d['nvn'], '.');
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['vorgangsnummer']='~&nbsp;<img src="'.$GLOBALS['gfxpath'].'dmy.png" height="1" width="'.(16*$lv).'"><img src="'.$GLOBALS['gfxpath'].'pt'.$d['art'].'.png" align="top">';
		if($d['nvn'] != $d['ovn'])
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['vorgangsnummer'] .= '&nbsp;<i class="s2 grey">('.no_null($d['ovn']).')</i>&nbsp;<font class="s1 lightgrey">&gt;</font>&nbsp;'.no_null($d['nvn']);
		else
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['vorgangsnummer'] .= '&nbsp;<font class="grey">'.no_null($d['ovn']).'</font>&nbsp;';

		// Vorgang (Name_Projekt)
		if(($d['md'] == 1) || ($d['md'] == 3))
			$np='~<font class="grey">'.trim($d['opn']).'</font>';
		else if($restore_inputs || (!$_POST['Button_Select'] && !$_POST['Button_Submit'] && ($err || ($osrc_id == $src_id))))
		{
			$np=trim($_POST['Name_Projekt'][$mc][$z]);
			$GLOBALS['_cppj']['data'][$id]['npn']=$np;
		}
		else
			$np=trim($d['npn']);
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['name_projekt']=$np;

		// (nv_text3)
		if($d['res'] || $d['sam'])
		{
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']='~';
			if($d['res'])
			{
				// Ressourcen ermitteln
				$id=$d['id'];
				if(substr($id,0,1) == 'N')
					$id=(int)substr($id,1);
				else
					$id=(int)$id;
				$sql ="SELECT m.personen_id, p.pname, p.vorname, p.inaktiv, d.maeintrittsdatum, d.maaustrittsdatum";
				$sql .= " FROM maplanung m, personen p LEFT JOIN madaten d ON (d.personen_id=p.personen_id AND d.mandanten_id=p.mandanten_id)";
				$sql .= " WHERE m.projekt_id=".$id." AND (m.sachmittel_id IS NULL OR m.sachmittel_id=0) AND m.mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$sql .= " AND p.personen_id=m.personen_id AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$sql .= " ORDER BY p.pname, p.vorname";
				$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);

				$ac=0;
				$tt='';
				$nsb=substr($d['nsb'],0,8);
				$nse=substr($d['nse'],0,8);
				foreach($tmp as $t)
				{
					$red=substr($t['maeintrittsdatum'],0,8);
					$rld=substr($t['maaustrittsdatum'],0,8);
					if(!strlen($rld))
						$rld='99999999';

					$rn=$t['pname'];
					if(strlen($t['vorname']))
						$rn .= ', '.$t['vorname'];
					$rn .= ' ('.get_entry($red,'datum');
					if($rld != '99999999')
						$rn .= '-'.get_entry($rld,'datum');
					$rn .= ')';
					if($t['inaktiv'])
						$rn .= $inakt;
//$rn .= ' -- $nsb='.$nsb.', $nse='.$nse.' -- $red='.$red.', $rld='.$rld;

					if(strlen($tt))
						$tt .= '<br />';
					if($t['inaktiv'] || ($red > $nse) || ($rld < $nsb))
					{
						$tt .= '<strike class=red>'.$rn.'</strike>';
						$ac++;
					}
					else
						$tt .= $rn;
				}

				$rc='black';
				if($ac)
					$rc='red';
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3'] .= '<span class="'.$rc.'" tooltip="'.$tt.'" style="border:1px solid #888888;border-radius:6px;text-align:center;padding:1px 3px;cursor:help;">R</span>';
				$pr++;
			}
			else
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3'] .= '<span style="border:1px solid transparent;color:transparent;text-align:center;padding:1px 3px;">R</span>';
			if($d['sam'])
			{
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3'] .= '&nbsp;<span style="border:1px solid #888888;border-radius:6px;text-align:center;padding:1px 2px;">M</span>';
				$ps++;
			}
		}
		else
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']='~<font class="s1 lightgrey">x</font>';

		// Soll-Beginn (Soll_Beginn)
		if(($d['md'] == 1) || ($d['md'] == 3))
		{
			if(($d['nsb'] != $d['osb']) || ($d['nse'] != $d['ose']))
				$GLOBALS['_cppj']['data'][$id]['md']=3;
			else
				$GLOBALS['_cppj']['data'][$id]['md']=1;
		}

		// Soll-Beginn (Soll_Beginn)
		$osbdf	= get_entry($d['osb'], 'datum');
		$nsbdf	= get_entry($d['nsb'], 'datum');
		if(($d['md'] == 1) || ($d['md'] == 3))
		{
			if($d['nsb'] != $d['osb'])
				$cls='blue';
			else
				$cls='grey';
		}
		else if(($d['nsb'] < $GLOBALS['_cppj']['info']['dsb']) || ($d['nsb'] > $GLOBALS['_cppj']['info']['dse']))
		{
			$cls='red';
			$te++;
		}
		else
			$cls='green';
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['soll_beginn']='~&nbsp;<font class="s2 grey">('.$osbdf.')</font>&nbsp;<br />&nbsp;<font class="'.$cls.'">'.$nsbdf.'</font>&nbsp;';

		// Soll-Ende (Soll_Ende)
		$osedf	= get_entry($d['ose'], 'datum');
		$nsedf	= get_entry($d['nse'], 'datum');
		if(($d['md'] == 1) || ($d['md'] == 3))
		{
			if($d['nse'] != $d['ose'])
				$cls='blue';
			else
				$cls='grey';
		}
		else if(($d['nse'] < $GLOBALS['_cppj']['info']['dsb']) || ($d['nse'] > $GLOBALS['_cppj']['info']['dse']))
		{
			$cls='red';
			$te++;
		}
		else
			$cls='green';
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['soll_ende']='~&nbsp;<font class="s2 grey">('.$osedf.')</font>&nbsp;<br />&nbsp;<font class="'.$cls.'">'.$nsedf.'</font>&nbsp;';

		$z++;
	}

	// Leerzeile mit Top border?
	if($z)
	{
		fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
		$l++;
	}

	// Buttons ausblenden?
	if($te)
	{
		$Button_Multi_nv=true;
		$Button_Zurueck_nv=true;
	}
	else
	{
		if(!$timeadjust)
			$Button_Zurueck_nv=true;
		$Button_Timeadjust1_nv=true;
		$Button_Timeadjust2_nv=true;
	}
	// Setting [pm_aendert_mp]: "Master projects can only be changed by the assigned project manager (except main administrator or general management)?"
	if((strtolower($GLOBALS['fxpglobals']['settings']['pm_aendert_mp']) == 'ja') && $GLOBALS['fxpglobals']['person'] && ($GLOBALS['fxpglobals']['user'] > 1) && !$GLOBALS['fxpglobals']['persdat']['roles'][2] && $GLOBALS['_cppj']['info']['pm'] && ($GLOBALS['_cppj']['info']['pm'] != $GLOBALS['fxpglobals']['person']))
		$Button_Timeadjust2_nv=true;

	if($dst_id)
	{
		// HINWEIS: Beim anpassen des Datums der Quellvorgänge werden alle kopierten Abhängigkeiten aufgelöst!
		if($GLOBALS['_cppj']['info']['sdp'] && !$Button_Timeadjust1_nv)
			error_msg(1857);
		// HINWEIS: Das übergeordnete Datum der Zielvorgänge kann wegen bestehender Abhängigkeiten nicht angepasst werden!
		if($GLOBALS['_cppj']['info']['ddp'] && !$Button_Timeadjust2_nv)
		{
			$Button_Timeadjust2_nv=true;
			error_msg(1858);
		}
		// HINWEIS: Der Int. Budgetrahmen ist um $var1 kleiner als das neue Int. gepl. Sollbudget! Wenn Sie auf [Neuanlage] klicken wird der Budgetrahmen automatisch erhöht.
		if(!$err && !$Button_Multi_nv && ($GLOBALS['_cppj']['info']['dbr'] < $GLOBALS['_cppj']['info']['nsbg']))
			error_msg(1863, false, get_entry($GLOBALS['_cppj']['info']['nsbg']-$GLOBALS['_cppj']['info']['dbr'], 'dezimal').' '.$GLOBALS['fxpglobals']['settings']['waehrung']);
	}
}

protectVar($GLOBALS['_cppj'], '_cppj');
?>