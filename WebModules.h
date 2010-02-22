/*
 * Copyright (C) 2004-2009  See the AUTHORS file for details.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#ifndef _WEBMODULES_H
#define _WEBMODULES_H

#include "Client.h"
#include "Template.h"
#include "HTTPSock.h"
#include "FileUtils.h"

class CWebSock;
class CModule;
class CWebSubPage;

typedef CSmartPtr<CWebSubPage> TWebSubPage;
typedef vector<TWebSubPage> VWebSubPages;

class CZNCTagHandler : public CTemplateTagHandler {
public:
	CZNCTagHandler(CWebSock& pWebSock);
	virtual ~CZNCTagHandler() {}

	virtual bool HandleTag(CTemplate& Tmpl, const CString& sName, const CString& sArgs, CString& sOutput);
private:
	CWebSock& m_WebSock;
};


class CWebSubPage {
public:
	CWebSubPage(const CString& sName, const CString& sTitle = "") : m_sName(sName), m_sTitle(sTitle) {
	}

	CWebSubPage(const CString& sName, const CString& sTitle, const VPair& vParams) : m_sName(sName), m_sTitle(sTitle), m_vParams(vParams) {}
	virtual ~CWebSubPage() {}

	void SetName(const CString& s) { m_sName = s; }
	void SetTitle(const CString& s) { m_sTitle = s; }
	void AddParam(const CString& sName, const CString& sValue) { m_vParams.push_back(make_pair(sName, sValue)); }

	const CString& GetName() const { return m_sName; }
	const CString& GetTitle() const { return m_sTitle; }
	const VPair& GetParams() const { return m_vParams; }

private:
	CString			m_sName;
	CString			m_sTitle;
	VPair			m_vParams;
};

class CWebAuth : public CAuthBase {
public:
	CWebAuth(CWebSock* pWebSock, const CString& sUsername, const CString& sPassword);
	virtual ~CWebAuth() {}

	void SetWebSock(CWebSock* pWebSock) { m_pWebSock = pWebSock; }
	void AcceptedLogin(CUser& User);
	void RefusedLogin(const CString& sReason);
private:
protected:
	CWebSock*	m_pWebSock;
};


class CWebSock : public CHTTPSock {
public:
	CWebSock(CModule* pModule);
	CWebSock(CModule* pModule, const CString& sHostname, unsigned short uPort, int iTimeout = 60);
	virtual ~CWebSock();

	virtual bool OnPageRequest(const CString& sURI, CString& sPageRet);

	virtual bool OnLogin(const CString& sUser, const CString& sPass);

	void ParsePath();	// This parses the path portion of the url into some member vars
	CModule* ResolveModule();

	//virtual bool PrintFile(const CString& sFileName, CString sContentType = "");
	bool PrintTemplate(const CString& sPageName, CString& sPageRet, CModule* pModule = NULL);
	bool PrintStaticFile(const CString& sPath, CString& sPageRet, CModule* pModule = NULL);

	bool AddModLoop(const CString& sLoopName, CModule& Module);
	void SetPaths(CModule* pModule, bool bIsTemplate = false);
	void SetVars();

	void FillErrorPage(CString& sPageRet, const CString& sError) {
		m_Template["Action"] = "error";
		m_Template["Title"] = "Error";
		m_Template["Error"] = sError;

		PrintTemplate("error", sPageRet);
	}

	void PrintErrorPage(const CString& sMessage);
	size_t AddError(const CString& sMessage);
	size_t AddSuccess(const CString& sMessage);

	// Setters
	void SetSessionUser(CUser* p) {
		m_pSessionUser = p;
	}
	// !Setters

	virtual Csock* GetSockObj(const CString& sHost, unsigned short uPort);
	bool IsAdmin() const;
	CUser* GetSessionUser() const;
	CString GetModWebPath(const CString& sModName) const;
	CString GetSkinPath(const CString& sSkinName) const;
	CModule* GetModule() const { return (CModule*) m_pModule; }
	size_t GetAvailSkins(vector<CFile>& vRet);
	CString GetSkinName() const;

private:
	CUser*					m_pSessionUser;
	bool					m_bPathsSet;
	CTemplate				m_Template;
	CSmartPtr<CAuthBase>	m_spAuth;
	CString                 m_sForceUser;   // Gets filled by ResolveModule()
	CString                 m_sModName;     // Gets filled by ResolveModule()
	CString                 m_sPath;        // Gets filled by ResolveModule()
	CString                 m_sPage;        // Gets filled by ResolveModule()
};

#endif // !_WEBMODULES_H
