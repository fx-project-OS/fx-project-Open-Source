<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : rechte_trans_rep_ges_pre.inc                                 //
// Version     : 21.2                                                         //
// Begin       : 2020-10-26                                                   //
// Last Change : 2020-10-26                                                   //
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
 * PF 148: User Overview
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */


// Client information array
$arr=fxf_getClientData();
fxDebug($arr['usr'],'$arr[\'usr\']', 3);

if(fxIsArray($arr['usr']))
{
	// No mask headline
	$maske239_nch=true;

	// Height
	$Uebersicht_height=22;

	// Mask variables
	$mc=$GLOBALS['_maskcounter'];
	$l=0;
	$GLOBALS['fxptdata']['lvalues'][$mc]=array();

	// Detailed user mode?
	$uid=0;
	$pft=false;
	if(fxIsArray($_GET) && isset($_GET['uid']) && $_GET['uid'])
	{
		$uid=(int)$_GET['uid'];
		// Get program function tree
		$pft=loadMenu(2);
fxDebug($pft,'$pft', 3);
	}

	// Texts
	$uinact	= meldung(1270, true);				// USER INACTIVE
	$pinact	= meldung(124, true);				// PERSON INACTIVE

	$emp	= meldung(794, true);				// - empty -

	$admin	= meldung(2200, true);				// User IS a <b>Administrator</b>.
	$madmin	= meldung(2201, true);				// User IS the <b>Main Administrator</b>.

	$ptype	= get_text(727, '20');				// Person Type
	$mlogin	= 'Logins';							// Logins
	$dlng	= meldung(2446, true);				// Standard language
	$dgrp	= meldung(1758, true);				// Standard Group
	$pgrp	= meldung(2127, true);				// Profile Groups

	$erl	= db_value("SELECT maart_name FROM ma_art WHERE maart_id=1 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id=0");	// Employee Role
	$std	= meldung(1700, true);				// Standard

	$sg_ms0	= meldung(2204, true);				// Standard Program
	$sg_ms1	= meldung(2208, true);				// Admin Programm
	$sg_ms2	= meldung(2210, true);				// Admin(1) Programm

	$ltt=explode('|',meldung(3043));			// Show detailed program rights for this user|Back to the total user overview

	// Array of all languages
	$larr=fxGetAllLanguages();
fxDebug($larr,'$larr: Array of all languages', 3);

	// Array of all possible person types
	$pta=fxf_getReferenceValues(30);
fxDebug($pta,'$pta: Array of all possible person types', 3);

	// Array of all user's employee roles
	$uearr=array();
	$sql  = "SELECT a.personen_id, a.mafaehigkeit_id, f.mafaehigkeit_name";
	$sql .= " FROM ma_art_faehigkeiten a LEFT JOIN ma_faehigkeit f ON (f.mafaehigkeit_id=a.mafaehigkeit_id AND f.maart_id=1 AND f.mandanten_id=0 AND f.id_sprache=".$GLOBALS['fxpglobals']['lang'].")";
	$sql .= " WHERE a.maart_id=1 AND a.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$sql .= " ORDER BY f.mafaehigkeit_name";
	$tmp=db_values($sql, '*');
	if(fxIsArray($tmp))
	{
		foreach($tmp as $t)
		{
			$pid=(int)$t['personen_id'];
			if(!isset($uearr[$pid]))
				$uearr[$pid]='';
			if(strlen($uearr[$pid]))
				$uearr[$pid] .= ', ';
			if(strlen($t['mafaehigkeit_name']))
				$uearr[$pid] .= '<b>'.$t['mafaehigkeit_name'].'</b>';
			else
				$uearr[$pid] .= '<i class=grey>('.$sid.')</i>';
		}
	}
fxDebug($uearr,'$uearr: Array of all user\'s employee roles', 3);
	if(fxIsArray($tmp))
	{
		foreach($tmp as $t)
			$brollen[(int)$t['personen_id']][(int)$t['mafaehigkeit_id']] = true;
	}

	// Cycle through all users
	$uc=0;
	foreach($arr['usr'] as $id => $a)
	{
		if($uid && ($id != $uid))
			continue;

		// Empty line with top border?
		if($l)
		{
			fxf_maskColumnBorder(array('Uebersicht'), 1, -1, $mc, $l, true);
			$l++;
		}
		// Back icon?
		else if($uid)
		{
			fxf_maskColumnBorder(array('Uebersicht'), 0, -1, $mc, $l, false);
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['uebersicht']='^class_transparent|~'.fieldLink('back','<div style="text-align:right;"><img class="op50" src="'.$GLOBALS['gfxpath'].'ret_b_40x40.png"></div>','?uid=0&keep_get=1',$ltt[1]);
			$l++;

			fxf_maskColumnBorder(array('Uebersicht'), 0, -1, $mc, $l, true);
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['uebersicht']='^class_transparent|~';
			$l++;
		}

		// ...Picture
		$pic=person_pic($a['pid'], 20, $a['gnd'], $GLOBALS['fxpglobals']['client'], false);

		// ...User Name
		$unm=$a['unm'];

		// ...Person Name
		$pnm=$a['pnm'];
		if(strlen($a['pfn']))
			$pnm .= ',&nbsp;'.$a['pfn'];

		// ...Background color
		if($id == 1)		// Main Admin
			$bc='3d8cb3';
		else if($a['adm'])	// Admin
			$bc='79aec7';
		else
			$bc='d2d2d2';

		// Text color
		$tc='black';
		if($a['sta'] != 1)
			$tc='red';

		// ...Inactive?
		$ina='';
		if($a['sta'] == -1)	// Person
			$ina='&nbsp;&nbsp;<i class="darkgrey">('.$pinact.')</i>';
		else if(!$a['sta'])	// User
			$ina='&nbsp;&nbsp;<i class="darkgrey">('.$uinact.')</i>';

		$tx='<br />&nbsp;&nbsp;'.$pic.'&nbsp;&nbsp;<b class="'.$tc.'">'.$unm.'</b>&nbsp;&nbsp;<span class="'.$tc.'">['.$pnm.']</span>'.$ina.'<br /><br />';
		if(!$uid)
			$tx=fieldLink('user'.$id,'<div style="position:relative;text-align:left;">'.$tx.'<img class="op50" src="'.$GLOBALS['gfxpath'].'arr_right_b_40x40.png" style="position:absolute;top:6px;right:4px;"></div>','?uid='.$id.'&keep_get=1',$ltt[0]);
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['uebersicht']='^col_'.$bc.'|~'.$tx;
		$l++;

		// ...E-Mail-Adresse
		if(strlen($a['eml']))
		{
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['uebersicht']='^class_?|~&nbsp;&nbsp;<a href="mailto:'.$a['eml'].'" target="_blank" style="border-bottom:1px dotted #888888;"><img src="'.$GLOBALS['gfxpath'].'eml.png" align="top">&nbsp;'.$a['eml'].'</a>';
			$l++;
		}

		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['uebersicht']='^class_?|~';
		$l++;

		// ...Admin
		if(($id == 1) || $a['adm'])
		{
			if($id == 1)
				$tx=$madmin;
			else
				$tx=$admin;
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['uebersicht']='^class_?|~&nbsp;&nbsp;&rarr;&nbsp;'.$tx;
			$l++;
		}

		// ...PersonType
		$tx= $ptype.':&nbsp;<b>'.$pta[$a['typ']].'</b>';
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['uebersicht']='^class_?|~&nbsp;&nbsp;&rarr;&nbsp;'.$tx;
		$l++;

		// ...Logins
		$lastlogin='';
		if($a['lin'] && $id)
		{
			$sql="SELECT zeitstempel FROM benutzer WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND benutzer_id=".$id;
			$lastlogin='&nbsp;&nbsp;<i class="grey">('.get_entry(db_value($sql), 'date').')</i>';
		}
		$tx=$mlogin.':&nbsp;<b>'.$a['lin'].'</b>'.$lastlogin;
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['uebersicht']='^class_?|~&nbsp;&nbsp;&rarr;&nbsp;'.$tx;
		$l++;

		// ...Default Language
		$tx=$dlng.':&nbsp;<img src="'.$larr[$a['lng']]['fs'].'" align="center">&nbsp;<b>'.$larr[$a['lng']]['tx'].'</b>';
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['uebersicht']='^class_?|~&nbsp;&nbsp;&rarr;&nbsp;'.$tx;
		$l++;

		// ...Standard Group
		$tx='<table border="0" cellpadding="0" cellspacing="0"><tr><td nowrap valign="top">&nbsp;&nbsp;&rarr;&nbsp;'.$dgrp.':&nbsp;</td><td valign="top">';
		if(($id == 1) || $a['adm'])
		{
			if($GLOBALS['fxpglobals']['client'] == 1)
				$tx .= '<b>'.$sg_ms2.'</b>, ';
			$tx .= '<b>'.$sg_ms1.'</b> + ';
		}
		$tx .= '<b>'.$sg_ms0.'</b></td></tr></table>';
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['uebersicht']='^class_?|~'.$tx;
		$l++;

		// ...Profile Groups
		if(fxIsArray($a['pga']))
		{
			$tx='<table border="0" cellpadding="0" cellspacing="0"><tr><td nowrap valign="top">&nbsp;&nbsp;&rarr;&nbsp;'.$pgrp.':&nbsp;</td><td valign="top">';
			$pc=0;
			foreach($a['pga'] as $pgr => $dmy)
			{
				$pc++;
				if($pc > 1)
				{
					if($pc == sizeof($a['pga']))
						$tx .= ' + ';
					else
						$tx .= ', ';
				}
				$tx .= '<b>'.$arr['pga'][$pgr]['nam'].'</b>';
			}
			$tx .= '</td></tr></table>';
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['uebersicht']='^class_?|~'.$tx;
			$l++;
		}

		// Employee Role
		$tx='<table border="0" cellpadding="0" cellspacing="0"><tr><td nowrap valign="top">&nbsp;&nbsp;&rarr;&nbsp;'.$erl.':&nbsp;</td><td valign="top">';
		if(isset($uearr[$a['pid']]))
			$tx .= $uearr[$a['pid']].' + ';
		$tx .= '<b>'.$std.'</b></td></tr></table>';
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['uebersicht']='^class_?|~'.$tx;
		$l++;

		// Program Functions
		if($uid && fxIsArray($pft) && fxIsArray($a['pfa']))
		{
			// Empty line with top border
			fxf_maskColumnBorder(array('Uebersicht'), 1, -1, $mc, $l, true);
			$l++;

			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['uebersicht']='^col_eaeaea|~<br />&nbsp;<b>'.meldung(42,true).'</b><br /><br />';
			$l++;

			$pfa=array();
			$ptc=0;
			foreach($pft as $pfi => $pta)
			{
				$pfa[$pfi]=false;
				if($pta['tr'] && (isset($a['pfa'][$pta['tr']]) || (fxIsArray($arr['pfa'][$pta['tr']]) && ($arr['pfa'][$pta['tr']]['ta'] == 1))))
				{
					$ptc++;
					$ia=explode('_', $pfi);
					$ii='';
					foreach($ia as $i)
					{
						if(strlen($ii))
							$ii .= '_';
						$ii .= $i;
						$pfa[$ii]=true;
					}
				}
			}

			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['uebersicht']='~<div style="position:relative;display:inline-block;"><img src="'.$GLOBALS['gfxpath'].'mt_st.png" border="0" style="position:absolute;left:0;top:-16px;"></div>';
			$l++;

			$pc=0;
			foreach($pfa as $pfi => $pfs)
			{
				$pc++;
				if($pfs)
				{
					$spi=strlen($pfi);
					$tl='';

					$ia=explode('_', $pfi);
					$ii='';
					$lv=0;
					foreach($ia as $i)
					{
						if(strlen($ii))
						{
							$lv++;
							$ii .= '_';
						}
						$ii .= $i;
						$si=strlen($ii);

						$ptr=$pft[$ii]['tr'];
						$pcc=0;
						$mtl='l';
						if($ptr)
							$mtl='l2';
						foreach($pfa as $cpfi => $cpfs)
						{
							$pcc++;
							if(($pcc >= $pc) && $cpfs)
							{
								if($ptr)
								{
									if($pcc > $pc)
									{
										if(strlen($cpfi) == $spi)
											$mtl='l1';
										break;
									}
								}
								else if(!$lv)
								{
									if(strlen($cpfi) == $si)
									{
										if($pcc == $pc)
											$mtl='l2';
										else if(strlen($cpfi) == $spi)
											$mtl='l1';
										else
											$mtl='l0';
									}
								}
								else
								{
									if(strlen($cpfi) == $si)
									{
										if($pcc == $pc)
											$mtl='l2';
										else if(substr($cpfi,0,3) == substr($pfi,0,3))
										{
											$mtl='l0';
											if(strlen($cpfi) == $spi)
												$mtl='l1';
										}
									}
								}
							}
						}
						$tl .= '<div style="position:relative;margin-right:16px;display:inline-block;" tooltip="'.$tt.'"><img src="'.$GLOBALS['gfxpath'].'mt_'.$mtl.'.png" border="0" style="position:absolute;left:0;top:-16px;"></div>&nbsp;';
					}

					$ptr=$pft[$pfi]['tr'];
					if($ptr)
					{
						if(fxIsArray($arr['pfa'][$ptr]) && ($arr['pfa'][$ptr]['ta'] == 1))
							$tl .= '<i class="blue">'.$pft[$pfi]['tx'].'</i>';
						else
							$tl .= $pft[$pfi]['tx'];
					}
					else
						$tl .= '<u class="grey">'.$pft[$pfi]['tx'].'</u>';

					$GLOBALS['fxptdata']['lvalues'][$mc][$l]['uebersicht']='~'.$tl;
					$l++;
				}
			}
		}

		$uc++;
	}

	// Empty line with top border?
	if($l)
	{
		fxf_maskColumnBorder(array('Uebersicht'), 1, -1, $mc, $l, true);
		$l++;
	}
}
?>