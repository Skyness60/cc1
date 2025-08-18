// ----------------------------
// File: src/front/parser/decl.rs
// ----------------------------
//! Parsing déclarations plus complet: types (int/char/void + signed/unsigned/long),
//! pointeurs, fonctions avec liste de paramètres (et varargs), initialisations simples,
//! prototypes et déclarations locales, tableaux, struct/union/enum, multiples déclarateurs.

use super::*;
use crate::front::token::TokenKind as K;

impl<'a> Parser<'a> {
    pub(crate) fn parse_type_specifier(&mut self) -> PResult<(CType, Span)> {
        // typedef-name ?
        {
            let t = self.ts.peek(0).map_err(|e| ParseError { msg: e.msg.clone(), span: e.span, is_fatal: false })?.clone();
            if t.kind == K::Ident && self.is_typedef_name(&t.lexeme) {
                let _ = self.ts.bump().map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?;
                return Ok((CType::Named(t.lexeme), t.span));
            }
        }
        // struct/union/enum ?
        if let Some((ty, sp)) = self.try_parse_tag_type()? { return Ok((ty, sp)); }
        // Spécificateurs simples: signed/unsigned, short/long, int/char/void/float/double
        let mut signed: Option<bool> = None;
        let mut long_flag = false; // for int or double
        let mut short_flag = false; // for int
        let mut base: Option<(CType, Span)> = None;
        let mut first_span: Option<Span> = None;
        loop {
            let t0 = self.ts.peek(0).map_err(|e| ParseError { msg: e.msg.clone(), span: e.span, is_fatal: false })?;
            match t0.kind {
                K::Signed => { let t = self.ts.bump().map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?; if signed.is_some() { return Err(ParseError { msg: "modificateur 'signed' en double".into(), span: t.span, is_fatal: false }); } signed = Some(true); first_span.get_or_insert(t.span); }
                K::Unsigned => { let t = self.ts.bump().map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?; if signed.is_some() { return Err(ParseError { msg: "modificateur de signe en double".into(), span: t.span, is_fatal: false }); } signed = Some(false); first_span.get_or_insert(t.span); }
                K::Long => {
                    let t = self.ts.bump().map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?;
                    if long_flag { return Err(ParseError { msg: "'long' en double".into(), span: t.span, is_fatal: false }); }
                    if short_flag { return Err(ParseError { msg: "'short' et 'long' ne se combinent pas".into(), span: t.span, is_fatal: false }); }
                    long_flag = true; first_span.get_or_insert(t.span);
                    // si base est déjà Double, on annotera plus tard (long double)
                }
                K::Short => {
                    let t = self.ts.bump().map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?;
                    if short_flag { return Err(ParseError { msg: "'short' en double".into(), span: t.span, is_fatal: false }); }
                    if long_flag { return Err(ParseError { msg: "'short' et 'long' ne se combinent pas".into(), span: t.span, is_fatal: false }); }
                    short_flag = true; first_span.get_or_insert(t.span);
                }
                K::Int => {
                    let t = self.ts.bump().map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?;
                    if base.is_some() { return Err(ParseError { msg: "spécificateur de type en double".into(), span: t.span, is_fatal: false }); }
                    base = Some((CType::Int { signed, long: long_flag, short: short_flag }, t.span));
                    first_span.get_or_insert(t.span);
                }
                K::Char => {
                    let t = self.ts.bump().map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?;
                    if base.is_some() { return Err(ParseError { msg: "spécificateur de type en double".into(), span: t.span, is_fatal: false }); }
                    if long_flag || short_flag { return Err(ParseError { msg: "'char' ne se combine pas avec short/long".into(), span: t.span, is_fatal: false }); }
                    base = Some((CType::Char { signed }, t.span));
                    first_span.get_or_insert(t.span);
                }
                K::Void => {
                    let t = self.ts.bump().map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?;
                    if base.is_some() || signed.is_some() || long_flag || short_flag { return Err(ParseError { msg: "'void' ne se combine pas avec signed/unsigned/short/long".into(), span: t.span, is_fatal: false }); }
                    base = Some((CType::Void, t.span));
                    first_span.get_or_insert(t.span);
                }
                K::Float => {
                    let t = self.ts.bump().map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?;
                    if base.is_some() { return Err(ParseError { msg: "spécificateur de type en double".into(), span: t.span, is_fatal: false }); }
                    if signed.is_some() || short_flag || long_flag { return Err(ParseError { msg: "'float' ne se combine pas avec signed/unsigned/short/long".into(), span: t.span, is_fatal: false }); }
                    base = Some((CType::Float, t.span));
                    first_span.get_or_insert(t.span);
                }
                K::Double => {
                    let t = self.ts.bump().map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?;
                    if base.is_some() { return Err(ParseError { msg: "spécificateur de type en double".into(), span: t.span, is_fatal: false }); }
                    if signed.is_some() || short_flag { return Err(ParseError { msg: "'double' ne se combine pas avec signed/unsigned/short".into(), span: t.span, is_fatal: false }); }
                    base = Some((CType::Double { long: long_flag }, t.span));
                    first_span.get_or_insert(t.span);
                }
                _ => break,
            }
        }
        let (ty, sp) = if let Some((ty, sp)) = base {
            (ty, sp)
        } else {
            if signed.is_some() || long_flag || short_flag {
                let sp = first_span.unwrap_or_else(|| {
                    // Fallback span si aucun modificateur n'a été consommé
                    self.ts.peek(0).map(|t| t.span).unwrap_or(Span { lo: 0, hi: 0, line: 1, col: 1 })
                });
                if short_flag && long_flag { return Err(ParseError { msg: "'short' et 'long' ne se combinent pas".into(), span: sp, is_fatal: false }); }
                (CType::Int { signed, long: long_flag, short: short_flag }, sp)
            } else {
                return Err(ParseError { msg: "spécificateur de type attendu (int/char/void/float/double)".into(), span: self.ts.peek(0).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?.span, is_fatal: false });
            }
        };
        Ok((ty, sp))
    }

    pub(crate) fn is_type_start(&mut self) -> PResult<bool> {
        let t0 = self.ts.peek(0).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?.clone();
        if t0.kind == K::Ident { return Ok(self.is_typedef_name(&t0.lexeme)); }
        Ok(matches!(
            t0.kind,
            // storage-class specifiers and type qualifiers can start a declaration
            K::Typedef | K::Extern | K::Static | K::Auto | K::Register | K::Const | K::Volatile |
            // primitive and aggregate specifiers
            K::Int | K::Char | K::Void | K::Signed | K::Unsigned | K::Long | K::Short | K::Float | K::Double | K::Struct | K::Union | K::Enum
        ))
    }

    // Parse un type-name (pour cast/sizeof): specifier + abstract-declarator optionnel (pointeurs/() params/[])
    pub(crate) fn parse_type_name(&mut self) -> PResult<CType> {
        let (base, _sp) = self.parse_type_specifier()?;
        // pointers with qualifiers
        let ty = self.apply_pointer_prefixes(base)?;
        // abstract declarator: either ( abstract-declarator ) then suffixes, or just suffixes
        let ty = if self.ts.matches(K::LParen).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? {
            let inner = self.parse_abstract_declarator(ty)?;
            let _ = self.ts.expect_kind(K::RParen).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?;
            self.parse_suffixes_on_type(inner)?
        } else {
            self.parse_suffixes_on_type(ty)?
        };
        Ok(ty)
    }

    fn parse_abstract_declarator(&mut self, base: CType) -> PResult<CType> {
        // allow leading pointer stars with qualifiers
        let mut ty = self.apply_pointer_prefixes(base)?;
        // optional nested (abstract-declarator)
        if self.ts.matches(K::LParen).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? {
            let inner = self.parse_abstract_declarator(ty)?;
            let _ = self.ts.expect_kind(K::RParen).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?;
            ty = inner;
        }
        Ok(ty)
    }

    pub(crate) fn parse_declaration_many(&mut self) -> PResult<Vec<Decl>> {
        // storage-class et qualifiers
        let mut storage: Option<StorageClass> = None;
        let mut quals: Vec<TypeQualifier> = Vec::new();
        loop {
            let t0 = self
                .ts
                .peek(0)
                .map_err(|e| ParseError { msg: e.msg.clone(), span: e.span, is_fatal: false })?
                .clone();
            match t0.kind {
                K::Typedef => { if storage.is_some() { return Err(ParseError { msg: "storage-class multiple".into(), span: t0.span, is_fatal: false }); } let _=self.ts.bump(); storage=Some(StorageClass::Typedef); }
                K::Extern => { if storage.is_some() { return Err(ParseError { msg: "storage-class multiple".into(), span: t0.span, is_fatal: false }); } let _=self.ts.bump(); storage=Some(StorageClass::Extern); }
                K::Static => { if storage.is_some() { return Err(ParseError { msg: "storage-class multiple".into(), span: t0.span, is_fatal: false }); } let _=self.ts.bump(); storage=Some(StorageClass::Static); }
                K::Auto => { if storage.is_some() { return Err(ParseError { msg: "storage-class multiple".into(), span: t0.span, is_fatal: false }); } let _=self.ts.bump(); storage=Some(StorageClass::Auto); }
                K::Register => { if storage.is_some() { return Err(ParseError { msg: "storage-class multiple".into(), span: t0.span, is_fatal: false }); } let _=self.ts.bump(); storage=Some(StorageClass::Register); }
                K::Const => { let _=self.ts.bump(); if quals.contains(&TypeQualifier::Const) { return Err(ParseError { msg: "qualifier 'const' en double".into(), span: t0.span, is_fatal: false }); } quals.push(TypeQualifier::Const); }
                K::Volatile => { let _=self.ts.bump(); if quals.contains(&TypeQualifier::Volatile) { return Err(ParseError { msg: "qualifier 'volatile' en double".into(), span: t0.span, is_fatal: false }); } quals.push(TypeQualifier::Volatile); }
                _ => break,
            }
        }
        let (base_ty, _sp) = self.parse_type_specifier()?;
        let mut decls = Vec::new();
        loop {
            let (name, name_sp, ty) = self.parse_declarator_full(base_ty.clone())?;
            let init = if self.ts.matches(K::Assign).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? { Some(self.parse_initializer()?) } else { None };
            decls.push(Decl { name, ty, init, storage, qualifiers: quals.clone(), bit_width: None, span: name_sp });
            if !self.ts.matches(K::Comma).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? { break; }
        }
        let _ = self.ts.expect_kind(K::Semicolon).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?;
        Ok(decls)
    }

    pub(crate) fn parse_declaration(&mut self) -> PResult<Decl> {
        let mut decls = self.parse_declaration_many()?;
        let first = decls.remove(0);
        // Si typedef, enregistrer tous les noms typedef de la liste
        if matches!(first.storage, Some(StorageClass::Typedef)) {
            self.add_typedef(&first.name);
            for d in decls.into_iter() { self.add_typedef(&d.name); }
        }
        Ok(first)
    }

    pub(crate) fn parse_top(&mut self) -> PResult<Top> {
        if let Some(tag_top) = self.try_parse_tag_decl_top()? { return Ok(tag_top); }
        // Handle storage-class and qualifiers at file scope
        let mut storage: Option<StorageClass> = None;
        let mut quals: Vec<TypeQualifier> = Vec::new();
        loop {
            let t0 = self
                .ts
                .peek(0)
                .map_err(|e| ParseError { msg: e.msg.clone(), span: e.span, is_fatal: false })?
                .clone();
            match t0.kind {
                K::Typedef => { if storage.is_some() { return Err(ParseError { msg: "storage-class multiple".into(), span: t0.span, is_fatal: false }); } let _=self.ts.bump(); storage=Some(StorageClass::Typedef); }
                K::Extern => { if storage.is_some() { return Err(ParseError { msg: "storage-class multiple".into(), span: t0.span, is_fatal: false }); } let _=self.ts.bump(); storage=Some(StorageClass::Extern); }
                K::Static => { if storage.is_some() { return Err(ParseError { msg: "storage-class multiple".into(), span: t0.span, is_fatal: false }); } let _=self.ts.bump(); storage=Some(StorageClass::Static); }
                K::Auto => { if storage.is_some() { return Err(ParseError { msg: "storage-class multiple".into(), span: t0.span, is_fatal: false }); } let _=self.ts.bump(); storage=Some(StorageClass::Auto); }
                K::Register => { if storage.is_some() { return Err(ParseError { msg: "storage-class multiple".into(), span: t0.span, is_fatal: false }); } let _=self.ts.bump(); storage=Some(StorageClass::Register); }
                K::Const => { let _=self.ts.bump(); if quals.contains(&TypeQualifier::Const) { return Err(ParseError { msg: "qualifier 'const' en double".into(), span: t0.span, is_fatal: false }); } quals.push(TypeQualifier::Const); }
                K::Volatile => { let _=self.ts.bump(); if quals.contains(&TypeQualifier::Volatile) { return Err(ParseError { msg: "qualifier 'volatile' en double".into(), span: t0.span, is_fatal: false }); } quals.push(TypeQualifier::Volatile); }
                _ => break,
            }
        }
        let (base_ty, _sp) = self.parse_type_specifier()?;
        let (name, name_sp, ty) = self.parse_declarator_full(base_ty.clone())?;
        // Old-style K&R: optional parameter declaration list before body
        if self.ts.check(K::LBrace).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? {
            let params = match &ty { CType::Function { params, .. } => params.clone(), _ => return Err(ParseError { msg: "définition: déclarateur de fonction attendu".into(), span: name_sp, is_fatal: false }) };
            let body = self.parse_compound_stmt()?;
            return Ok(Top::Func(FuncDef { name, ty, params, body, span: name_sp }));
        } else if matches!(ty, CType::Function { .. }) && self.is_type_start().unwrap_or(false) {
            // consume parameter declarations (K&R style)
            while self.is_type_start()? { let _ = self.parse_declaration_many()?; }
            let body = self.parse_compound_stmt()?;
            let params = match &ty { CType::Function { params, .. } => params.clone(), _ => vec![] };
            return Ok(Top::Func(FuncDef { name, ty, params, body, span: name_sp }));
        }
        let mut decls = Vec::new();
        let init = if self.ts.matches(K::Assign).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? { Some(self.parse_initializer()?) } else { None };
        decls.push(Decl { name, ty, init, storage, qualifiers: quals.clone(), bit_width: None, span: name_sp });
        while self.ts.matches(K::Comma).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? {
            let (n2, sp2, ty2) = self.parse_declarator_full(base_ty.clone())?;
            let init2 = if self.ts.matches(K::Assign).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? { Some(self.parse_initializer()?) } else { None };
            decls.push(Decl { name: n2, ty: ty2, init: init2, storage, qualifiers: quals.clone(), bit_width: None, span: sp2 });
        }
        let _ = self.ts.expect_kind(K::Semicolon).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?;
        if matches!(storage, Some(StorageClass::Typedef)) { for d in &decls { self.add_typedef(&d.name); } }
        if decls.len() == 1 { Ok(Top::Decl(decls.last().cloned().unwrap())) } else { Ok(Top::Decls(decls)) }
    }

    fn apply_pointer_prefixes(&mut self, mut ty: CType) -> PResult<CType> {
        // Parse '*' with optional qualifiers (const/volatile) after each star
        loop {
            if !self.ts.matches(K::Star).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? { break; }
            let mut pquals: Vec<TypeQualifier> = Vec::new();
            loop {
                let t0 = self.ts.peek(0).map_err(|e| ParseError { msg: e.msg.clone(), span: e.span, is_fatal: false })?.clone();
                match t0.kind {
                    K::Const => { let _=self.ts.bump(); if pquals.contains(&TypeQualifier::Const) { return Err(ParseError { msg: "qualifier 'const' en double sur pointeur".into(), span: t0.span, is_fatal: false }); } pquals.push(TypeQualifier::Const); }
                    K::Volatile => { let _=self.ts.bump(); if pquals.contains(&TypeQualifier::Volatile) { return Err(ParseError { msg: "qualifier 'volatile' en double sur pointeur".into(), span: t0.span, is_fatal: false }); } pquals.push(TypeQualifier::Volatile); }
                    _ => break,
                }
            }
            ty = CType::Pointer { of: Box::new(ty), qualifiers: pquals };
        }
        Ok(ty)
    }

    fn parse_declarator_full(&mut self, base: CType) -> PResult<(String, Span, CType)> {
        // préfixe pointeurs (avec qualifiers)
        let mut ty = self.apply_pointer_prefixes(base)?;
        // direct declarator: ident | '(' ... ')'
        let t0 = self.ts.peek(0).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?;
        let (name, sp) = if t0.kind == K::Ident {
            self.parse_ident()?
        } else if t0.kind == K::LParen {
            // cas parenthésé, typiquement pointeur de fonction: int (*name)(...)
            let _ = self.ts.bump().map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?; // '('
            // optionnel: pointeurs à l’intérieur (avec qualifiers)
            ty = self.apply_pointer_prefixes(ty)?;
            let (name, sp) = self.parse_ident()?;
            let _ = self.ts.expect_kind(K::RParen).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?;
            (name, sp)
        } else {
            return Err(ParseError { msg: "identifiant ou déclarateur attendu".into(), span: t0.span, is_fatal: false });
        };
        // suffixes: fonctions/arrays (inclure K&R ident-lists)
        loop {
            if self.ts.matches(K::LParen).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? {
                // K&R old-style: ident list (a, b, c)
                let t1 = self.ts.peek(0).map_err(|e| ParseError { msg: e.msg.clone(), span: e.span, is_fatal: false })?.clone();
                if t1.kind == K::RParen {
                    let _ = self.ts.bump();
                    ty = CType::Function { ret: Box::new(ty), params: vec![], varargs: false, prototype: false };
                } else if t1.kind == K::Ident && !self.is_typedef_name(&t1.lexeme) {
                    let mut params: Vec<Param> = Vec::new();
                    loop {
                        let (pname, _sp) = self.parse_ident()?;
                        params.push(Param { name: Some(pname), ty: CType::Int { signed: None, long: false, short: false } });
                        if self.ts.matches(K::Comma).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? { continue; }
                        break;
                    }
                    let _ = self.ts.expect_kind(K::RParen).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?;
                    ty = CType::Function { ret: Box::new(ty), params, varargs: false, prototype: false };
                } else {
                    let (params, varargs, proto) = self.parse_param_list()?;
                    ty = CType::Function { ret: Box::new(ty), params: params.clone(), varargs, prototype: proto };
                }
            } else if self.ts.matches(K::LBracket).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? {
                let mut sz: Option<usize> = None;
                let t0 = self.ts.peek(0).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?;
                if t0.kind == K::IntLit { let tnum = self.ts.bump().map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?; if let Ok(v) = tnum.lexeme.parse::<usize>() { sz = Some(v); } }
                let _ = self.ts.expect_kind(K::RBracket).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?;
                ty = CType::Array { of: Box::new(ty), size: sz };
            } else { break; }
        }
        Ok((name, sp, ty))
    }

    fn parse_param_list(&mut self) -> PResult<(Vec<Param>, bool, bool)> {
        let mut params = Vec::new();
        let mut varargs = false;
        // prototype flag: true if we parsed a prototype parameter-type-list (including void)
        let prototype = true;
        if self.ts.matches(K::RParen).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? { return Ok((params, varargs, false)); }
        if self.ts.matches(K::Void).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? { self.ts.expect_kind(K::RParen).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?; return Ok((params, varargs, true)); }
        loop {
            if self.ts.matches(K::Ellipsis).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? { varargs = true; self.ts.expect_kind(K::RParen).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?; break; }
            // optional qualifiers/storage-class (e.g., const, volatile, register) before type specifier
            loop {
                let t0 = self.ts.peek(0).map_err(|e| ParseError { msg: e.msg.clone(), span: e.span, is_fatal: false })?.clone();
                match t0.kind { K::Const | K::Volatile | K::Register => { let _ = self.ts.bump(); } _ => break }
            }
            let (base, _sp) = self.parse_type_specifier()?;
            // utiliser un sous-déclarateur pour paramètres (supporte (*name)(...))
            let (pname_opt, pty) = self.parse_param_declarator(base)?;
            params.push(Param { name: pname_opt, ty: pty });
            if self.ts.matches(K::Comma).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? { continue; }
            self.ts.expect_kind(K::RParen).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?; break;
        }
        Ok((params, varargs, prototype))
    }

    fn parse_param_declarator(&mut self, base: CType) -> PResult<(Option<String>, CType)> {
        // autoriser: int a | char *s | int (*cb)(int) | int arr[10]
        let mut ty = self.apply_pointer_prefixes(base)?;
        let t0 = self.ts.peek(0).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?;
        // direct: ident
        if t0.kind == K::Ident {
            let (name, _sp) = self.parse_ident()?;
            let ty2 = self.parse_suffixes_on_type(ty)?;
            return Ok((Some(name), ty2));
        }
        // parenthésé: (*ident) ...
        if t0.kind == K::LParen {
            let _ = self.ts.bump().map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?;
            ty = self.apply_pointer_prefixes(ty)?;
            let (name, _sp) = self.parse_ident()?;
            let _ = self.ts.expect_kind(K::RParen).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?;
            let ty2 = self.parse_suffixes_on_type(ty)?;
            return Ok((Some(name), ty2));
        }
        // aucun nom: paramètre de type simple
        Ok((None, ty))
    }

    fn parse_suffixes_on_type(&mut self, mut ty: CType) -> PResult<CType> {
        loop {
            if self.ts.matches(K::LParen).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? {
                let (params, varargs, proto) = self.parse_param_list()?;
                ty = CType::Function { ret: Box::new(ty), params: params.clone(), varargs, prototype: proto };
            } else if self.ts.matches(K::LBracket).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? {
                let mut sz: Option<usize> = None;
                let t0 = self.ts.peek(0).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?;
                if t0.kind == K::IntLit { let tnum = self.ts.bump().map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?; if let Ok(v) = tnum.lexeme.parse::<usize>() { sz = Some(v); } }
                let _ = self.ts.expect_kind(K::RBracket).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?;
                ty = CType::Array { of: Box::new(ty), size: sz };
            } else { break; }
        }
        Ok(ty)
    }

    fn parse_initializer(&mut self) -> PResult<Initializer> {
        // { init-list } | expression
        if self.ts.matches(K::LBrace).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? {
            let mut elems = Vec::new();
            if !self.ts.check(K::RBrace).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? {
                loop {
                    elems.push(self.parse_initializer()?);
                    if self.ts.matches(K::Comma).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? {
                        if self.ts.check(K::RBrace).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? { break; }
                        continue;
                    }
                    break;
                }
            }
            let _ = self.ts.expect_kind(K::RBrace).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?;
            Ok(Initializer::List(elems))
        } else {
            Ok(Initializer::Expr(self.parse_expr()?))
        }
    }

    // struct/union/enum parsing
    fn try_parse_tag_type(&mut self) -> PResult<Option<(CType, Span)>> {
        let t0 = self.ts.peek(0).map_err(|e| ParseError { msg: e.msg.clone(), span: e.span, is_fatal: false })?;
        let (kw, cons): (Option<K>, fn(Option<String>, Option<Vec<Decl>>) -> CType) = match t0.kind {
            K::Struct => (Some(K::Struct), |tag, fields| CType::Struct { tag, fields }),
            K::Union  => (Some(K::Union),  |tag, fields| CType::Union  { tag, fields }),
            K::Enum   => (Some(K::Enum),   |_tag, _| CType::Enum { tag: _tag, items: None }),
            _ => (None, |_, _| CType::Void),
        };
        if kw.is_none() { return Ok(None); }
        let tkw = self.ts.bump().map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?;
        let mut tag: Option<String> = None;
        if self.ts.check(K::Ident).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? { let (n, _) = self.parse_ident()?; tag = Some(n); }
        
        // définition optionnelle { ... }
        if self.ts.matches(K::LBrace).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? {
            if tkw.kind == K::Enum {
                // enum items: ident [= const-expr] (, ...)
                let mut items: Vec<(String, Option<Expr>)> = Vec::new();
                if !self.ts.check(K::RBrace).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? {
                    loop {
                        let (name, _) = self.parse_ident()?;
                        let val = if self.ts.matches(K::Assign).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? { 
                            // Parse assignment expression (excludes comma operator to avoid confusion with enum separator)
                            Some(self.parse_assign_expr()?) 
                        } else { 
                            None 
                        };
                        items.push((name, val));
                        if self.ts.matches(K::Comma).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? { 
                            if self.ts.check(K::RBrace).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? { break; } 
                            continue; 
                        }
                        break;
                    }
                }
                let _ = self.ts.expect_kind(K::RBrace).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?;
                return Ok(Some((CType::Enum { tag, items: Some(items) }, tkw.span)));
            } else {
                // struct/union fields: liste de déclarations (type + déclarateurs, finies par ';')
                let mut fields: Vec<Decl> = Vec::new();
                while !self.ts.check(K::RBrace).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? {
                    let (field_base, _sp) = self.parse_type_specifier()?;
                    // Unnamed bit-field: ':' width;
                    if self.ts.matches(K::Colon).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? {
                        let width_expr = self.parse_expr()?;
                        fields.push(Decl { name: String::new(), ty: field_base.clone(), init: None, storage: None, qualifiers: vec![], bit_width: Some(width_expr), span: _sp });
                        let _ = self.ts.expect_kind(K::Semicolon).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?;
                        continue;
                    }
                    loop {
                        let (fname, fsp, fty) = self.parse_declarator_full(field_base.clone())?;
                        // bit-field optionnel
                        let bw = if self.ts.matches(K::Colon).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? { Some(self.parse_expr()?) } else { None };
                        // interdiction d'initialiseur dans les champs
                        if self.ts.matches(K::Assign).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? {
                            return Err(ParseError { msg: "initialiseur de champ 'struct/union' interdit en C89".into(), span: fsp, is_fatal: false });
                        }
                        fields.push(Decl { name: fname, ty: fty, init: None, storage: None, qualifiers: vec![], bit_width: bw, span: fsp });
                        if !self.ts.matches(K::Comma).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? { break; }
                    }
                    let _ = self.ts.expect_kind(K::Semicolon).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?;
                }
                let _ = self.ts.expect_kind(K::RBrace).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })?;
                // Enregistrer le tag comme défini
                if let Some(ref tag_name) = tag {
                    match tkw.kind {
                        K::Struct => self.add_struct_tag(tag_name),
                        K::Union => self.add_union_tag(tag_name),
                        _ => {}
                    }
                }
                return Ok(Some((cons(tag, Some(fields)), tkw.span)));
            }
        }
        // Pas de définition { ... }, donc référence ou forward declaration
        if let Some(ref tag_name) = tag {
            // Vérifier si le tag existe déjà
            let exists = match tkw.kind {
                K::Struct => self.is_struct_tag(tag_name),
                K::Union => self.is_union_tag(tag_name),
                K::Enum => self.is_enum_tag(tag_name),
                _ => false,
            };
            
            // Si le tag n'existe pas encore, l'enregistrer comme forward declaration
            if !exists {
                match tkw.kind {
                    K::Struct => self.add_struct_tag(tag_name),
                    K::Union => self.add_union_tag(tag_name),
                    K::Enum => self.add_enum_tag(tag_name),
                    _ => {}
                }
            }
        }
        
        // Retourner le type (avec ou sans tag, avec fields=None)
        Ok(Some((cons(tag, None), tkw.span)))
    }

    fn try_parse_tag_decl_top(&mut self) -> PResult<Option<Top>> {
        // Déclarations de tag seules: struct S; | struct S { ... };
        let t0 = self.ts.peek(0).map_err(|e| ParseError { msg: e.msg.clone(), span: e.span, is_fatal: false })?;
        if !matches!(t0.kind, K::Struct | K::Union | K::Enum) { return Ok(None); }
        
        // Lookahead : struct/union/enum [ident] (; | {...})
        let mut lookahead = 1;
        if let Ok(t) = self.ts.peek(lookahead) {
            if t.kind == K::Ident {
                lookahead += 1;
            }
        }
        // Vérifier si c'est une déclaration de tag (soit ; soit { pour définition)
        if let Ok(t) = self.ts.peek(lookahead) {
            if t.kind == K::Semicolon || t.kind == K::LBrace {
                // OK, c'est bien une déclaration de tag, on peut parser
                if let Some((ty, sp)) = self.try_parse_tag_type()? {
                    if self.ts.matches(K::Semicolon).map_err(|e| ParseError { msg: e.msg, span: e.span, is_fatal: false })? {
                        return Ok(Some(Top::Tag(ty, sp)));
                    }
                    // Si pas de ';', c'est probablement une définition avec {}, 
                    // qui a été parsée par try_parse_tag_type
                    return Ok(Some(Top::Tag(ty, sp)));
                }
            }
        }
        Ok(None)
    }
}
