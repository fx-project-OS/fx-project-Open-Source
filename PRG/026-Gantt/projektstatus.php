<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : projektstatus.inc                                            //
// Version     : 21.2                                                         //
// Begin       : 2020-12-23                                                   //
// Last Change : 2020-12-23                                                   //
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
 * PF 26: Gantt
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

$callAJAX=false;
if(fxIsArray($_GET) && isset($_GET['pid']) && isset($_GET['auto']) && isset($_GET['lvl']) && isset($_GET['jplan']))
	$callAJAX=true;

// Include basics
if($callAJAX)
{
	$von='';
	$bis='';

	$ProjektID=(int)$_GET['pid'];
	$auto=(int)$_GET['auto'];
	$lvl=666;
	$jplan=(int)$_GET['jplan'];

	if(strlen($_GET['lvl']))
		$lvl=abs((int)$_GET['lvl']);
}

// Database string length function
$slen="LENGTH";
if($GLOBALS['_dbtypes'][$GLOBALS['fxpglobals']['dbparam']['dbtype']]['type'] == 'sqlsrv')
	$slen="LEN";

// Selected project number
$pd_vn='';
if($ProjektID > 0)
{
	$sql="SELECT vorgangsnummer FROM projekte WHERE projekt_id=".(int)$ProjektID." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$pd_vn=db_value($sql);
}

// Planned project timespan
$pvon='';
$pbis='';

$sql  = "SELECT MIN(z.soll_beginn) AS sb, MAX(z.soll_ende) AS se FROM projekte p, zeitdaten z";
$sql .= " WHERE p.vorgangsnummer LIKE '".$pd_vn."%' AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
if($lvl < 666)
	$sql .= " AND ".$slen."(p.vorgangsnummer)<".(($lvl+1)*5);
$sql .= " AND z.zeit_id=p.zeit_id AND z.zeitart=102 AND z.mandanten_id=".$GLOBALS['fxpglobals']['client'];
$pln=db_values($sql);
//fxDebug($pln, $sql);
if(fxIsArray($pln))
{
	$pvon=substr($pln['sb'],0,8);
	$pbis=substr($pln['se'],0,8);
}

// Planned milstone timespan
$mvon='';
$mbis='';

$sql  = "SELECT MIN(soll_ende) AS mb, MAX(soll_ende) AS me FROM meilensteine";
$sql .= " WHERE projekt_id IN (SELECT projekt_id FROM projekte WHERE vorgangsnummer LIKE '".$pd_vn."%' AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
if($lvl < 666)
	$sql .= " AND ".$slen."(vorgangsnummer)<".(($lvl+1)*5);
$sql .= ") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
$mst=db_values($sql);
//fxDebug($mst, $sql);
if(fxIsArray($mst))
{
	$mvon=substr($mst['mb'],0,8);
	$mbis=substr($mst['me'],0,8);
}

// Actual project timespan
$ivon='';
$ibis='';

if(!$jplan)
{
	$sql  = "SELECT MIN(ist_beginn) AS ib, MAX(ist_ende) AS ie FROM zeitdaten";
	$sql .= " WHERE projekt_id IN (SELECT projekt_id FROM projekte WHERE vorgangsnummer LIKE '".$pd_vn."%' AND projektart=".FXP_PRJ_TASK." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	if($lvl < 666)
		$sql .= " AND ".$slen."(vorgangsnummer)<".(($lvl+1)*5);
	$sql .= ") AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.",".FXP_TE_TRAVELEXP.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$ist=db_values($sql);
//fxDebug($ist, $sql);
	if(fxIsArray($ist))
	{
		$ivon=substr($ist['ib'],0,8);
		$ibis=substr($ist['ie'],0,8);
	}
}

// Start date, if no user input or automatic timespan
if(!strlen($von) || $auto)
{
	$von=$pvon;
	if($mvon)
		$von=min($von,$mvon);
	if($ivon)
		$von=min($von,$ivon);
}
// Check user input
else if(!$callAJAX)
	$von=checkdatum('Zeitspanne_von');

// End date, if no user input or automatic timespan
if(!strlen($bis) || $auto)
{
	$bis=$pbis;
	if($mbis)
		$bis=max($bis,$mbis);
	if($ibis)
		$bis=max($bis,$ibis);
}
// Check user input
else if(!$callAJAX)
	$bis=checkdatum('Zeitspanne_bis');
//echo('$pvon='.$pvon.', $pbis='.$pbis.' - $ivon='.$ivon.', $ibis='.$ibis.' -&gt; $von='.$von.', $bis='.$bis.'<br>');

// Get standard start and end dates
if(!strlen($von))
{
	$von=fxNow();

	$tag=(int)substr($von,6,2);
	$monat=(int)substr($von,4,2);
	$jahr=(int)substr($von,0,4);

	// Month
	$mo_tage=array(1 => 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31);
	$lj=fx_date('L', fx_mktime(12, 0, 0, 1, 1, $jahr));
	if($lj)
		$mo_tage[2]=29;

	// Start week day
	$wtg=fx_date('w', fx_mktime(12, 0, 0, $monat, $tag, $jahr)) - 1;
	if($wtg < 0)
		$wtg += 7;
	$stg=(int)($GLOBALS['fxpglobals']['settings']['ersterwt'] - 1);
	if($stg < 0)
		$stg += 7;

	while($wtg != $stg)
	{
		$tag--;
		if($tag < 1)
		{
			$monat--;
			if($monat < 1)
			{
				$monat=12;
				$jahr--;
			}
			$tag=$mo_tage[$monat];
		}
		$wtg--;
		if($wtg < 0)
			$wtg += 7;
	}
	$von=substr('0000'.$jahr, -4).substr('00'.$monat, -2).substr('00'.$tag, -2);
}

if(!strlen($bis))
{
	$jahr=(int)substr($von,0,4);
	$monat=(int)substr($von,4,2);
	$tag=(int)substr($von,6,2);

	$bis=fx_date('Ymd', fx_mktime(12, 0, 0, $monat, $tag+$GLOBALS['fxpglobals']['settings']['stdzeitspanne']-1, $jahr));
}

if($callAJAX)
	echo(get_entry($von, "datum").'|'.get_entry($bis, "datum"));
?>