<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : rechte_trans_pre.inc                                         //
// Version     : 24.1                                                         //
// Begin       : 2020-10-12                                                   //
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
 * PF 56: Profile Program Assignment - Program Function Rights - Main Mask
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

if(fxIsArray($arr) && fxIsArray($spa))
{
	$mc=$GLOBALS['_maskcounter'];

	// Get action codes for each program function and profile
	$pta=array();
	$sql="SELECT benutzergr_id, trans_id, rechte_trans_id, aktionscode FROM rechte_trans WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY benutzergr_id, trans_id";
	$tmp=db_values($sql,'*');
fxDebug($tmp,$sql, 3);
	if(fxIsArray($tmp))
	{
		foreach($tmp as $t)
		{
			$pid=$t['benutzergr_id'];
			if(!isset($pta[$pid]))
				$pta[$pid]=array();
			$pta[$pid][$t['trans_id']]=array('rt'=>$t['rechte_trans_id'], 'ac'=>$t['aktionscode']);
		}
	}
fxDebug($pta,'$pta', 3);

	// Get menu
	$vt=array();
	// Load authorized program functions
	$pft=loadMenu(2);
	if(fxIsArray($pft))
	{
		foreach($pft as $tk => $tc)
		{
			if($tc['tr'])
				$vt[$tc['tr']]=true;
		}
	}
//fxDebug($vt,'$vt', 0);

	// Load all possible program functions?
	$disp_all=strtolower(substr((string)$GLOBALS['fxpglobals']['settings']['alle_tr_anzeig'],0,1));
//$disp_all='n';
	if($disp_all !== 'n')
		$pft=loadMenu(true,false);
	if(fxIsArray($pft))
	{
fxDebug($pft,'Program function tree', 3);
fxDebug($arr,'$arr', 3);

		$csv='';

		// Alias trans
		$atlt=array();
		$attl=array();
		$ata=getAliasTrans($pft);
fxDebug($ata,'$ata', 2);

		$cbinactive='<input type="checkbox" disabled style="margin:2px;">';

		// Tooltips
		$tta_det=explode('|',meldung(3034));	// Detailierte Aktionsberechtigungen für alle Programmfunktionen einblenden|Detailierte Aktionsberechtigungen für alle Programmfunktionen ausblenden|Detailierte Aktionsberechtigungen für diese Programmfunktion einblenden|Detailierte Aktionsberechtigungen für diese Programmfunktion ausblenden
		$tta_act=explode('|',meldung(3035));	// Alle möglichen Aktionen dieser Programmfunktion erlauben/verweigern|Anzeige-Modus dieser Programmfunktion erlauben/verweigern|Neuanlage-Modus dieser Programmfunktion erlauben/verweigern|Änderungs-Modus dieser Programmfunktion erlauben/verweigern|Lösch-Modus dieser Programmfunktion erlauben/verweigern
		$tta_spr=explode('|',meldung(3037));	// Programm kann nicht zugeordnet werden, da dieses Programm nur im Hauptmandant 1 erlaubt ist|Programm ist ein Basisprogramm für folgende Alternativprogramme|Programm ist ein Alternativprogramm für folgendes Basisprogramm|Programm kann nicht zugeordnet werden, da dieses Programm ein noch nicht gekauftes oder eingebundenes Sonderprogramm ist

		// Add profile columns
		$csl='"Programmname"';
		$apd=array();
		$fields=array();
		$columns=array('Transn');
		if(isset($spa[1]))	// fx-Admin
		{
			$apd[0]=0;
			$fields['admin']=array('literal'=>$ladmin, 'data'=>array('anzeigeart'=>5));	// 5=Read-Only
			$columns[]='admin';

			$csl .= ';"'.$ladmin.'"';
		}
		foreach($spa as $apdid => $sname)
		{
			if(($apdid != 0) && ($apdid != 1))
			{
				$augar=$arr['pga'][$apdid];
				if($apdid < 0)
					$pid='m'.abs($apdid);
				else
					$pid=(string)$apdid;

				$fieldname='profile_'.$pid;

				$apd[$apdid]=0;

				$fields[$fieldname]=array('literal'=>$augar['nam'], 'data'=>array('anzeigeart'=>11));	// 11=Checkbox
				$columns[]=$fieldname;

				$csl .= ';"'.$augar['nam'].'"';
			}
		}
		$csv .= $csl.$GLOBALS['nl_b'];
fxDebug($apd,'$apd', 3);
fxDebug($fields,'$fields', 2);

		maskAddNewListFields(191,$fields);

		// Profile column width
		$pcw=max(80, min(196, floor(512/sizeof($apd))));
		$ccw=fxGetHelveticaStringWidth('W', 8.0, 'bold', 'px');
		$mcl=floor($pcw/$ccw);
fxDebug(array('$pcw'=>$pcw, '$ccw'=>$ccw, '$mcl'=>$mcl),'Profile column width', 3);

		$l=0;
		$apop=0;
		$apcl=0;
		$sdisp=false;
		if(!$GLOBALS['fxpglobals']['person'])
			$sdisp=true;
		foreach($pft as $p)
		{
			if(!$p['lv'] && ($p['tx'] != 'Sysadmin'))
				$sdisp=true;
			if(!$sdisp)
				continue;

			$ctr=$p['tr'];

			// Empty line with top border?
			if($l && !$p['lv'])
			{
				fxf_maskColumnBorder($columns, 1, -1, $mc, $l, true);
				$atlt[$l]=0;
				$l++;
			}

			// Validity
			$atrans=0;
			$btrans=array();
			$valid=1;
			if($ctr)
			{
				if(((($p['ta'] == 27) || ($p['ta'] == 30)) && ($GLOBALS['fxpglobals']['client'] > 1)) || ($p['ta'] > 30))
					$valid=0;
				else if(isset($GLOBALS['sppdir'][$ctr]) || strlen((string)$p['eb']))
				{
					$valid=-(int)$p['eb'];
					if(function_exists('fxsppCheckEBTR') && fxsppCheckEBTR($ctr))
						$valid=1;
				}
				$attl[$ctr]=$l;
			}
			if(isset($ata[$ctr]) && $ata[$ctr]['source'])	// Alias trans to basis
				$atrans=$ata[$ctr]['source'];
			if(isset($ata[$ctr]) && $ata[$ctr]['alias'])	// Base trans for alias
				$btrans=$ata[$ctr]['alias'];
			$atlt[$l]=$atrans;

			// Class
			$pass=0;
			if(!$ctr || ($p['ta'] == 1))
				$pass=1;
			else
			{
				foreach($sarr as $apdid => $dummy)
				{
					if($apdid)
					{
						for($c=1; $c<5; $c++)
						{
							if(($p['ac'] & $bit[$c]) && fxIsArray($pta[$apdid]) && fxIsArray($pta[$apdid][$ctr]) && ($pta[$apdid][$ctr]['ac'] & $bit[$c]))
							{
								$pass=2;
								break;
							}
						}
						if($pass)
							break;
					}
				}
				if(!$pass && (($p['ta'] == 1) || ($p['ta'] == 17) || ($p['ta'] == 20) || (($p['ta'] == 30) && ($GLOBALS['fxpglobals']['client'] == 1))))
					$pass=3;
			}

			$cls='^class_?';
			// ...Mask generated programs = blue
			if($valid < 1)
				$cls .= ' grey';
			else if(!$pass)
				$cls .= ' red';
			else if($ctr < 0)
				$cls .= ' lightblue';
			else if(!$ctr)
				$cls .= ' grey';
			else if($pass < 2)
				$cls .= ' green';
			else if($pass > 2)
				$cls .= ' blue';

			if($valid < 1)
				$clsc='^class_cfp'.(($l+1)%2);
			else if($pass)
				$clsc='^class_rv'.(1+$l%2);
			else
				$clsc='^class_cnb'.(($l+1)%2);

			$csl='"'.str_repeat('...', $p['lv']);
			if($ctr)
				$csl .= '#'.$ctr.': ';
			$csl .= $p['tx'].'"';

			// Program function
			$pop=0;
			$pcl=0;
			if($ctr)
			{
				// Profile groups
				foreach($apd as $apdid => $dummy)
				{
					$csl .= ';';

					if(!$apdid)	// Admin
					{
						$GLOBALS['fxptdata']['lvalues'][$mc][$l]['admin']=$clsc.'|~';
						if(($p['ta'] == 1) || ($p['ta'] == 17) || ($p['ta'] == 20) || (($p['ta'] == 30) && ($GLOBALS['fxpglobals']['client'] == 1)))
						{
							$GLOBALS['fxptdata']['lvalues'][$mc][$l]['admin'] .= '<input type="checkbox" class="blue" disabled checked style="opacity:0.5;margin:2px;">';
							$csl .= '1';
						}
					}
					else
					{
						if($apdid < 0)
							$pid='m'.abs($apdid);
						else
							$pid=(string)$apdid;

						$fieldname='profile_'.$pid;

						if(($valid > 0) || !$GLOBALS['fxpglobals']['person'])
						{
							$cbr=0;
							$cbs=0;
							$cbf=0;
							$rtr=0;
							$cbrc=0;
							$cbsc=0;
							for($c=1; $c<5; $c++)
							{
								// Bit: 1=Display (2), 2=Create (4), 3=Edit (8) + 4=Delete (16)
								if($p['ac'] & $bit[$c])
								{
									$cs=false;
									if($err && fxIsArray($sa) && fxIsArray($sa[$pid]) && ($sa[$pid][$ctr] & $bit[$c]))
									{
										$cbs += $bit[$c];
										$cbsc++;
										$cs=true;
									}

									if(fxIsArray($pta[$apdid]) && fxIsArray($pta[$apdid][$ctr]))
									{
										$rtr=$pta[$apdid][$ctr]['rt'];
										if(!$err && ($pta[$apdid][$ctr]['ac'] & $bit[$c]))
										{
											$cbs += $bit[$c];
											$cbsc++;
											$cs=true;
										}
									}

									// Set "Open Source" and "Extension packages"
									if(!$cs && !fxIsArray($pta[$apdid]) && ($fields[$fieldname]['literal'] !== NULL))
									{
										$fl3=substr((string)$fields[$fieldname]['literal'],0,3);
										if(($p['pb'] === NULL) || !strlen((string)$p['pb']))
											$pb3='00 ';
										else
											$pb3=substr('0'.(int)$p['pb'],-2).' ';
										if($fl3 === $pb3)
											$cbf += $bit[$c];
									}
									$cbr += $bit[$c];
									$cbrc++;
								}
							}
							$cbr=(int)$cbr;
							$cbs=(int)$cbs;
							$cbf=(int)$cbf;

							$GLOBALS['fxptdata']['lvalues'][$mc][$l][$fieldname]=$clsc.'|~';
							if($p['ta'] == 1)
							{
								$GLOBALS['fxptdata']['lvalues'][$mc][$l][$fieldname] .= '<input type="checkbox" class="red" disabled checked style="opacity:0.5;margin:2px;">';
								$csl .= '1';
							}
							else
							{
								$cchk=0;
								$dispall='block';
								$dispdet='none';
								if(($cbr == $cbs) || ($cbr == $cbf))
								{
									$checked=1;
									$cchk=1;
								}
								else
								{
									$checked=0;
									if(($cbs && ($cbr != $cbs)) || ($cbf && ($cbr != $cbf)))
									{
										$dispall='none';
										$dispdet='block';
									}
								}

								if($cbrc > 1)
								{
									$pop++;
									if(($cbs && ($cbr != $cbs)) || ($cbf && ($cbr != $cbf)))
										$pcl++;
								}

								// Alias field for checkbox popup
								$GLOBALS['cb0_'.$pid.'_alias']=$fieldname;

								$GLOBALS['fxptdata']['lvalues'][$mc][$l][$fieldname] .= '<div id="allcb_'.$pid.'['.$mc.']['.$l.']" style="position:relative;display:'.$dispall.';">';
								// Tooltip: Alle möglichen Aktionen dieser Programmfunktion erlauben/verweigern
								$GLOBALS['fxptdata']['lvalues'][$mc][$l][$fieldname] .= fieldCheckbox('cb0_'.$pid.'['.$mc.']['.$l.']', 1, $checked, false, false, false, $tta_act[0], '', '', false, 'margin:2px;');
								$GLOBALS['fxptdata']['lvalues'][$mc][$l][$fieldname] .= '</div>';

								if($cbrc > 1)
								{
									$GLOBALS['fxptdata']['lvalues'][$mc][$l][$fieldname] .= '<div id="detcb_'.$pid.'['.$mc.']['.$l.']" style="position:relative;display:'.$dispdet.';">';
									for($c=1; $c<5; $c++)
									{
										if($cbr & $bit[$c])
										{
											$checked=0;
											if(($cbs & $bit[$c]) || ($cbf & $bit[$c]))
											{
												$checked=1;
												$cchk=1;
											}
											// Tooltip: XYZ-Modus dieser Programmfunktion erlauben/verweigern
											$GLOBALS['fxptdata']['lvalues'][$mc][$l][$fieldname] .= fieldCheckbox('cb'.$c.'_'.$pid.'['.$mc.']['.$l.']', 1, $checked, false, false, false, $tta_act[$c], '', '', false, 'margin:2px;');
										}
										else
											$GLOBALS['fxptdata']['lvalues'][$mc][$l][$fieldname] .= $cbinactive;
									}
									$GLOBALS['fxptdata']['lvalues'][$mc][$l][$fieldname] .= '</div>';
								}

								// ...Hidden
								if($err && fxIsArray($_POST['info_'.$pid]) && fxIsArray($_POST['info_'.$pid][$mc]) && isset($_POST['info_'.$pid][$mc][$l]))
									$GLOBALS['fxptdata']['lvalues'][$mc][$l][$fieldname] .= fieldHidden('info_'.$pid.'['.$mc.']['.$l.']', $_POST['info_'.$pid][$mc][$l]);
								else
									$GLOBALS['fxptdata']['lvalues'][$mc][$l][$fieldname] .= fieldHidden('info_'.$pid.'['.$mc.']['.$l.']', $ctr.'_'.$cbr.'_'.$cbs.'_'.$rtr.'_'.$p['tx']);

								if($cchk)
									$csl .= '1';
							}
						}
						else
							$GLOBALS['fxptdata']['lvalues'][$mc][$l][$fieldname]=$clsc.'|~<input type="checkbox" disabled style="opacity:0.5;margin:2px;">';
					}
				}
			}
			else
				fxf_maskEmptyLine($columns, $mc, $l, $clsc.'|~');

			// Program function name
			$tx=$p['tx'];
			if(!$valid)			// Program cannot be assigned because this programm is only valid for the main client 1
			{
				$tt=$tta_spr[0];
				$tx .= '&nbsp;<b class="red s2" style="cursor:help;">(1)</b>';
			}
			else if($valid < 0)	// Program cannot be assigned because this program is a special program that has not been authorised
			{
				$tt=$tta_spr[3];
				$tx .= '&nbsp;<b class="grey s2" style="cursor:help;">['.abs($valid).']</b>';
			}
			else if($atrans || $btrans)
			{
				if($atrans)
					$abt=$atrans;
				else
					$abt=$btrans;
				$tt='<ul style=padding-left:10px;padding-top:4px;>';
				if(!fxIsArray($abt))
					$abt=array((int)$abt=>true);
				foreach($abt as $atr => $dummy)
					$tt .= '<li>&nbsp;<span class=blue>'.trPath($atr).'</span></li>';
				$tt .= '</ul>';

				if($atrans)	// Program is an alias program for the following basis program
				{
					$tt=$tta_spr[2].':'.$tt.'</b>';
					$tx .= '&nbsp;&nbsp;<b class="red s4" style="cursor:help;">&uarr;</b>';
				}
				else		// Program is a basis program for the following alias programs
				{
					$tt=$tta_spr[1].':'.$tt.'</b>';
					$tx .= '&nbsp;&nbsp;<b class="green s4" style="cursor:help;">&darr;</b>';
				}
			}
			else
				$tt='';
			if(strlen((string)$tt))
				$tt='" tooltip="'.$tt;
			if(!$ctr)
				$tx='<u>'.$tx.'</u>';
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['transn']  = $cls.$tt.'|~<div style="position:relative;margin-left:'.($p['lv']*20).'px;padding-right:32px;">';
			if($ctr && ($pop || $pcl))
			{
				$apop += $pop;
				$apcl += $pcl;
				if($pcl == $pop)
				{
					$dispop='none';
					$dispcl='block';
				}
				else
				{
					$dispop='block';
					$dispcl='none';
				}
				// Tooltips: Detailierte Aktionsberechtigungen für diese Programmfunktion einblenden/ausblenden
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['transn'] .= '<img id="enlcb_'.$l.'" src="'.$GLOBALS['gfxpath'].'fl_op.png" tooltip="'.$tta_det[2].'" style="position:absolute;right:0;top:2px;cursor:pointer;display:'.$dispop.';">';
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['transn'] .= '<img id="shrcb_'.$l.'" src="'.$GLOBALS['gfxpath'].'fl_cl.png" tooltip="'.$tta_det[3].'" style="position:absolute;right:0;top:2px;cursor:pointer;display:'.$dispcl.';">';
			}
			$tnr='';
			if($ctr && (($GLOBALS['__debug']['debugmode']&FXP_DEBUG_MIN) || ($GLOBALS['__debug']['debugmode']&FXP_DEBUG_DEF) || ($GLOBALS['__debug']['debugmode']&FXP_DEBUG_MAX)))
				$tnr='<i class="blue">['.$ctr.']</i>&nbsp;&nbsp;';
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['transn'] .= $tnr.$tx.'</div>';

			$l++;

			$csv .= $csl.$GLOBALS['nl_b'];
		}

		// Empty line with top border?
		if($l)
		{
			fxf_maskColumnBorder($columns, 1, -1, $mc, $l, true);
			$l++;
		}

		// Save profile assignments as CSV
//echo('<pre>'.fxHtmlEncode($csv).'</pre>');
		$csvpath=$GLOBALS['tmppath'].'profiles-'.substr('000000'.$GLOBALS['fxpglobals']['client'],-6).'-'.$GLOBALS['datetime'].'.csv';
		fxSave($csvpath,$csv);
		$li=fieldLinkImage('icon_download', 'im_dl', 'loader.php?url=download.inc&tp=csv&fn='.urlencode($csvpath), 'Download: &quot;<b>CSV</b>&quot;');
		$GLOBALS['fxpvars']['ibuffer'][]=$li;

		// Header for program function column with global open/close icon
		if($apcl >= $apop/2)
		{
			$dispop='none';
			$dispcl='block';
		}
		else
		{
			$dispop='block';
			$dispcl='none';
		}
		$Transn_lit  = '<div style="position:relative;">';
		// Tooltips: Detailierte Aktionsberechtigungen für alle Programmfunktionen einblenden/ausblenden
		$Transn_lit .= '<img id="aenlcb" src="'.$GLOBALS['gfxpath'].'fl_op.png" tooltip="'.$tta_det[0].'" style="position:absolute;right:0;top:2px;cursor:pointer;display:'.$dispop.';">';
		$Transn_lit .= '<img id="ashrcb" src="'.$GLOBALS['gfxpath'].'fl_cl.png" tooltip="'.$tta_det[1].'" style="position:absolute;right:0;top:2px;cursor:pointer;display:'.$dispcl.';">';
		$Transn_lit .= get_text(991,'15').'</div>';
		$Transn_lit .= fieldHidden('mc', $mc);

		// Header for profile columns
		$hla=array();
		$hlc=0;
		foreach($apd as $apdid => $points)
		{
			$nm=$arr['pga'][$apdid]['nam'];
			$hla[$apdid]='';
			while(($nm !== NULL) && strlen((string)$nm))
			{
				if(strlen((string)$hla[$apdid]))
					$hla[$apdid] .= '<br />';
				$hla[$apdid] .= trim(substr((string)$nm,0,$mcl));
				$nm=trim(substr((string)$nm,$mcl));
			}
			$hlc=max($hlc,1+substr_count($hla[$apdid],'<'));
		}
		$GLOBALS['admin_lit']='<div class="blue" style="position:relative;width:'.$pcw.'px;height:'.($hlc*16).'px;margin-bottom:4px;">'.$ladmin.'</div><div style="float:left;width:'.$pcw.'px;"><font class="lightblue" style="font-size:8pt;font-weight:normal;">1x</font></div>';
		foreach($apd as $apdid => $points)
		{
			$augar=$arr['pga'][$apdid];

			if($augar['act'])
				$cl='darkgrey';
			else
				$cl='lightergrey';

			if($apdid < 0)
				$pid='m'.abs($apdid);
			else
				$pid=(string)$apdid;

			$fieldname='profile_'.$pid;
			$GLOBALS[$fieldname.'_lit']='<div class="'.$cl.'" style="position:relative;width:'.$pcw.'px;height:'.($hlc*16).'px;margin-bottom:4px;">'.$hla[$apdid].'</div><div style="float:left;width:'.$pcw.'px;"><font class="lightergrey" style="font-size:8pt;font-weight:normal;">'.$augar['cnt'].'x</font></div>';
		}
	}
}
?>