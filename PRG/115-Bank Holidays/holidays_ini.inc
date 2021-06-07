<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : holidays_ini.inc                                             //
// Version     : 21.1                                                         //
// Begin       : 2020-09-24                                                   //
// Last Change : 2020-09-24                                                   //
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
 * PF 115: Bank Holidays - Initialization file
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

// Holiday types (Value from table "references", field 591)
define('FXP_HT_PUBLIC', 796);		// Public Holiday
define('FXP_HT_RELIGIOUS', 797);	// Religious Holiday
define('FXP_HT_COMPANY', 2284);		// Company Vacation
define('FXP_HT_OTHER', 1016);		// Other Holiday

// Basic variables
//fxDebug('_POST');
$year=0;
$country='';
if(fxIsArray($_POST))
{
	if(isset($_POST['year']))
		$year=(int)$_POST['year'];
	else if(isset($GLOBALS['fxptdata']['backup']['year']))
		$year=(int)$GLOBALS['fxptdata']['backup']['year'];
	if(isset($_POST['country']))
		$country=trim($_POST['country']);
	else if(isset($GLOBALS['fxptdata']['backup']['country']))
		$country=trim($GLOBALS['fxptdata']['backup']['country']);

	if(isset($_POST['pyear']) || isset($_POST['nyear']))
	{
		if(isset($_POST['pyear']))
			$year--;
		else
			$year++;
		$_POST['auto']=true;
	}
}
setDateLimits();
$minyear=(int)$GLOBALS['fxpvars']['date_minmax']['min'];
$maxyear=(int)$GLOBALS['fxpvars']['date_minmax']['max'];
if(!$minyear)
	$minyear=(int)substr($GLOBALS['date'],0,4)-100;
if(!$maxyear)
	$maxyear=(int)substr($GLOBALS['date'],0,4)+50;
if($year)
	$year=min(max($year, $minyear), $maxyear);
$GLOBALS['fxptdata']['backup']['year']=$year;
$GLOBALS['fxptdata']['backup']['country']=$country;

// Validation array
$hva=array();

// Get all languages
$la=fxGetAllLanguages();

// Allways call validation file, even for action mode "Delete"
if($db_update)
	$force_val=true;

// Load all holiday definitions
$hda=array();
if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
{
	$sha=fxScanDir($GLOBALS['lngpath'], false, true, '/^hd-.+\.txt$/i');
//fxDebug($sha,'$sha', 0);
	if($sha['file_entries'] > 0)
	{
		foreach($sha['files'] as $fn => $fp)
		{
			// Default array with path, flag and days
			$fa=explode('-', substr($fn,3,-4));
			$fc=trim($fa[0]);
			$hda[$fc]=array('path'=>$fp, 'flag'=>'', 'days'=>array());
			if(sizeof($fa) > 1)
			{
				$fg=trim($fa[1]);
				if(file_exists($GLOBALS['gfxpath'].'flg_'.$fg.'.png'))
					$hda[$fc]['flag']=$GLOBALS['gfxpath'].'flg_'.$fg.'.png';
			}
			else
			{
				$fg='00';
				if(file_exists($GLOBALS['gfxpath'].'flg_'.$fg.'.png'))
					$hda[$fc]['flag']=$GLOBALS['gfxpath'].'flg_'.$fg.'.png';
			}

			// Load and analyze text file
			$ta=fxLoad($fp, 1);
//fxDebug($ta,'$ta', 0);
			if(fxIsArray($ta))
			{
				foreach($ta as $tl)
				{
					$tl=trim($tl);
					$hp=strpos($tl,'#');
					if($hp !== false)
						$tl=trim(substr($tl,0,$hp));
					if(strlen($tl))
					{
						$cp=strpos($tl,':');
						if($cp)
						{
							$hd=trim(substr($tl,0,$cp));
							$hda[$fc]['days'][$hd]=trim(substr($tl,$cp+1));
//echo($fc.' - <b>'.$hd.'</b>: &quot;'.$hda[$fc]['days'][$hd].'&quot; &rarr; '.fxf_calcDate($hda[$fc]['days'][$hd]).'<br />');
						}
					}
				}
			}

			if(!sizeof($hda[$fc]['days']))
				unset($hda[$fc]);
		}

		if(sizeof($hda))
			ksort($hda, SORT_STRING);
	}
//fxDebug($hda,'$hda', 0);
}
?>