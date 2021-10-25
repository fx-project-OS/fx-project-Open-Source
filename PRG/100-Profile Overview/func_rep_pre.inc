<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : func_rep_pre.inc                                             //
// Version     : 21.2                                                         //
// Begin       : 2020-10-23                                                   //
// Last Change : 2020-10-23                                                   //
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
 * PF 100: Profile Overview
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

// Get program function tree
$pft=loadMenu(2);
fxDebug($pft,'$pft', 3);

// Client information array
$arr=fxf_getClientData();
fxDebug($arr,'$arr', 3);
fxDebug($arr['pga'],'$arr[\'pga\']', 2);

// inactive
$inact='&nbsp;&nbsp;<i class="red" style="font-size:smaller;">(*'.meldung(299).')</i>';	// (*inaktiv)

if(fxIsArray($pft) && fxIsArray($arr['pga']))
{
	$mc=$GLOBALS['_maskcounter'];
	$li=0;
	$GLOBALS['fxptdata']['lvalues'][$mc]=array();

	// Height
	$Transn_height=22;

	foreach($arr['pga'] as $pid => $pga)
	{
		if($li)
		{
			fxf_maskColumnBorder(array('Transn'), 1, -1, $mc, $li, true);
			$li++;
		}

		// Profile
		$ph=$pga['nam'];
		$pc='';
		if($pga['cnt'])
			$ph .= '&nbsp;&nbsp;<font class="black" style="font-size:smaller;">['.$pga['cnt'].'x]</font>';
		if(!$pga['act'])
			$ph .= $inact;
		if(!$pga['act'] || !$pga['cnt'])
			$pc .= ' op25';
		fxf_listSectHeader($mc, $li, '&nbsp;<font class="s4b'.$pc.'">'.$ph.'</font>', 'transn', 1, -2, 1);
		$li++;

		// Program Functions
		if(sizeof($pga['pfa']))
		{
			$pfa=array();
			$ptc=0;
			foreach($pft as $pfi => $pta)
			{
				$pfa[$pfi]=false;
				if($pta['tr'] && (isset($pga['pfa'][$pta['tr']]) || (fxIsArray($arr['pfa'][$pta['tr']]) && ($arr['pfa'][$pta['tr']]['ta'] == 1))))
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

			$GLOBALS['fxptdata']['lvalues'][$mc][$li]['transn']='~<div style="position:relative;display:inline-block;"><img src="'.$GLOBALS['gfxpath'].'mt_st.png" border="0" style="position:absolute;left:0;top:-16px;"></div>';
			$li++;

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

					$GLOBALS['fxptdata']['lvalues'][$mc][$li]['transn']='~'.$tl;
					$li++;
				}
			}
		}
	}
	if($li)
	{
		fxf_maskColumnBorder(array('Transn'), 1, -1, $mc, $li, true);
		$li++;
	}
}
?>