/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#pragma once

#include <svx/optgrid.hxx>

#include "scdllapi.h"
#include "optutil.hxx"
#include "global.hxx"

// View options

enum ScViewOption
{
    VOPT_FORMULAS = 0,
    VOPT_NULLVALS,
    VOPT_SYNTAX,
    VOPT_NOTES,
    VOPT_VSCROLL,
    VOPT_HSCROLL,
    VOPT_TABCONTROLS,
    VOPT_OUTLINER,
    VOPT_HEADER,
    VOPT_GRID,
    VOPT_GRID_ONTOP,
    VOPT_HELPLINES,
    VOPT_ANCHOR,
    VOPT_PAGEBREAKS,
    VOPT_SUMMARY,
    VOPT_THEMEDCURSOR,
    VOPT_CLIPMARKS,
};

enum ScVObjType
{
    VOBJ_TYPE_OLE = 0,
    VOBJ_TYPE_CHART,
    VOBJ_TYPE_DRAW
};

#define MAX_OPT             sal_uInt16(VOPT_CLIPMARKS)+1
#define MAX_TYPE            sal_uInt16(VOBJ_TYPE_DRAW)+1

// SC_STD_GRIDCOLOR is obsolete since tdf#152184 since GridColor == COL_AUTO
// converts now to either light or dark but still used on options > view > visual aids
#define SC_STD_GRIDCOLOR    COL_LIGHTGRAY

// SvxGrid options with standard operators

class ScGridOptions : public SvxOptionsGrid
{
public:
                ScGridOptions()  {}
                ScGridOptions( const SvxOptionsGrid& rOpt ) : SvxOptionsGrid( rOpt ) {}

    void                    SetDefaults();
    bool                    operator== ( const ScGridOptions& rOpt ) const;
    bool                    operator!= ( const ScGridOptions& rOpt ) const { return !(operator==(rOpt)); }
};

// Options - View

class SC_DLLPUBLIC ScViewOptions
{
public:
                ScViewOptions();
                ScViewOptions( const ScViewOptions& rCpy );
                ~ScViewOptions();

    void                    SetDefaults();

    void                    SetOption( ScViewOption eOpt, bool bNew )    { aOptArr[eOpt] = bNew; }
    bool                    GetOption( ScViewOption eOpt ) const         { return aOptArr[eOpt]; }

    void                    SetObjMode( ScVObjType eObj, ScVObjMode eMode ) { aModeArr[eObj] = eMode; }
    ScVObjMode              GetObjMode( ScVObjType eObj ) const             { return aModeArr[eObj]; }

    void                    SetGridColor( const Color& rCol, const OUString& rName ) { aGridCol = rCol; aGridColName = rName;}
    Color const &           GetGridColor( OUString* pStrName = nullptr ) const;

    const ScGridOptions&    GetGridOptions() const                      { return aGridOpt; }
    void                    SetGridOptions( const ScGridOptions& rNew ) { aGridOpt = rNew; }
    std::unique_ptr<SvxGridItem> CreateGridItem() const;

    const OUString& GetColorSchemeName() const { return sColorSchemeName; }
    void SetColorSchemeName( const OUString& rName ) { sColorSchemeName = rName; }

    const Color& GetDocColor() const { return aDocCol; }
    void SetDocColor(const Color& rDocColor) { aDocCol = rDocColor; }

    ScViewOptions&          operator=  ( const ScViewOptions& rCpy );
    bool                    operator== ( const ScViewOptions& rOpt ) const;
    bool                    operator!= ( const ScViewOptions& rOpt ) const { return !(operator==(rOpt)); }

private:
    bool            aOptArr     [MAX_OPT];
    ScVObjMode      aModeArr    [MAX_TYPE];
    Color           aGridCol;
    OUString        aGridColName;
    ScGridOptions   aGridOpt;
    // The name of the color scheme
    OUString sColorSchemeName = "Default";
    // The background color of the document
    Color aDocCol;
};

// Item for the options dialog - View

class SC_DLLPUBLIC ScTpViewItem final : public SfxPoolItem
{
public:
                ScTpViewItem( const ScViewOptions& rOpt );
                virtual ~ScTpViewItem() override;

    ScTpViewItem(ScTpViewItem const &) = default;
    ScTpViewItem(ScTpViewItem &&) = default;
    ScTpViewItem & operator =(ScTpViewItem const &) = delete; // due to SfxPoolItem
    ScTpViewItem & operator =(ScTpViewItem &&) = delete; // due to SfxPoolItem

    virtual bool            operator==( const SfxPoolItem& ) const override;
    virtual ScTpViewItem*   Clone( SfxItemPool *pPool = nullptr ) const override;

    const ScViewOptions&    GetViewOptions() const { return theOptions; }

private:
    ScViewOptions   theOptions;
};

// CfgItem for View options

class ScViewCfg : public ScViewOptions
{
    ScLinkConfigItem    aLayoutItem;
    ScLinkConfigItem    aDisplayItem;
    ScLinkConfigItem    aGridItem;

    DECL_LINK( LayoutCommitHdl, ScLinkConfigItem&, void );
    DECL_LINK( DisplayCommitHdl, ScLinkConfigItem&, void );
    DECL_LINK( GridCommitHdl, ScLinkConfigItem&, void );

    static css::uno::Sequence<OUString> GetLayoutPropertyNames();
    static css::uno::Sequence<OUString> GetDisplayPropertyNames();
    static css::uno::Sequence<OUString> GetGridPropertyNames();

public:
            ScViewCfg();

    void            SetOptions( const ScViewOptions& rNew );
};

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
