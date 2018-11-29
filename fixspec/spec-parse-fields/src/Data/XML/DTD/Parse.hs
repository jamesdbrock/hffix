{-# LANGUAGE OverloadedStrings #-}
------------------------------------------------------------------------------
-- |
-- Module      :  Data.XML.DTD.Parse
-- Copyright   :  Suite Solutions Ltd., Israel 2011
--                (c) 2013 Montez Fitzpatrick
--
-- Maintainer  :  Montez Fitzpatrick <montezf@gmail.com>
-- Portability :  portable
--
-- This module provides a "Data.Attoparsec.Text" parser for XML
-- Document Type Declaration (DTD) documents. A higher-level interface
-- that implements parameter entity resolution is also provided.

{-
Copyright (c) 2011 Suite Solutions Ltd., Israel. All rights reserved.

For licensing information, see the BSD3-style license in the file
license.txt that was originally distributed by the author together
with this file.
-}

module Data.XML.DTD.Parse
  ( -- * Parsing a DTD
    dtd
  , parseDTD
  , parseDTDWithExtern
  , SymTable

    -- * Top-level DTD structure
  , textDecl
  , dtdComponent

    -- * Entity declarations and references
  , entityDecl
  , entityValue
  , pERef
  , notation
  , notationSrc

    -- * Element declarations
  , elementDecl
  , contentDecl
  , contentModel
  , repeatChar

    -- * Attribute declarations
  , attList
  , attDecl
  , attType
  , attDefault

    -- * Declarations of comments and processing instructions
  , instruction
  , comment

    -- * Parsing combinators for general DTD syntax
  , externalID
  , name
  , nameSS
  , quoted
  )
  where

import Data.XML.DTD.Types
import Data.XML.Types (ExternalID(PublicID, SystemID),
  Instruction(Instruction))
import Data.Attoparsec.Text (Parser, try, satisfy, takeTill,
  anyChar, char, digit, inClass)
import qualified Data.Attoparsec.Text as A -- for takeWhile
import Data.Attoparsec.Text.Lazy (parse, Result(Done, Fail), maybeResult)
import Data.Attoparsec.Combinator (many', manyTill, choice, sepBy1)
import Data.Functor ((<$>))
import Control.Applicative (Applicative, pure, optional, (<*>), (<*), (*>), (<|>))
import Control.Monad (guard, join)
import Data.Text (Text)
import Data.Char (isSpace)
import qualified Data.Text as T
import qualified Data.Text.Lazy as L
import qualified Data.Map as M
import Data.Maybe (fromMaybe, catMaybes)
import Data.List (groupBy)

-- | Parse a DTD from lazy 'L.Text' while fully resolving the values
-- of all parameter entities whose values are provided internally in
-- the DTD. If the syntax of the DTD is invalid, all declarations up
-- to the first invalid one are returned.
parseDTD :: L.Text -> DTD
parseDTD = parseDTDWithExtern M.empty

-- | Parse a DTD from lazy 'L.Text' while fully resolving the values
-- of parameter entities. The given table of values is used to resolve
-- external parameter entities.
--
-- If you need information from the DTD itself to look up the external
-- entities, such as system and public IDs, you might be able to get
-- the information by applying 'parseDTD' to the DTD or part of it for
-- an initial parse.
parseDTDWithExtern :: SymTable -> L.Text -> DTD
parseDTDWithExtern ext = continue . parse (skipWS *> textDecl <* skipWS)
  where
    continue (Done inp decl) = DTD (Just decl) $ parseCmps ext M.empty inp
    continue (Fail inp _ _)  = DTD Nothing     $ parseCmps ext M.empty inp

-- | A pre-parsed component of the DTD. Pre-parsing separates
-- components that need parameter entity replacement from those that
-- do not.
data PreParse =
     PPERef PERef
   | PInstruction Instruction
   | PComment Text
   | PMarkup [MarkupText]
  deriving (Eq, Show)

-- | Markup text is interspersed quoted 'Text', unquoted 'Text', and
-- parameter entity references.
data MarkupText = MTUnquoted Text | MTQuoted Text | MTPERef PERef
  deriving (Eq, Show)

-- | A symbol table for internal parameter entity resolution.
type IntSymTable = M.Map Text (Maybe [EntityValue])

-- | A symbol table for external parameter entity resolution.  The
-- symbol table maps strict 'Text' names to lazy 'L.Text' values.
-- Typically, the values will have been retrieved from an external
-- resource such as a file or URL.
type SymTable = M.Map Text L.Text

-- | Parse the components of a DTD, given the current symbol table of
-- parameter entities.
parseCmps :: SymTable -> IntSymTable -> L.Text -> [DTDComponent]
parseCmps ext int = handlePre . parse (preparse <* skipWS)
  where
    handlePre (Done c (PPERef r)) = handlePERef ext int c r
    handlePre (Done c (PComment t)) = DTDComment t : parseCmps ext int c
    handlePre (Done c (PInstruction i)) = DTDInstruction i :
                                          parseCmps ext int c
    handlePre (Done c (PMarkup m)) = handleMarkup ext int c m
    handlePre _ = []

-- | To handle a pre-parsed parameter entity reference, try to resolve
-- it. If it resolves, push the resolved text back into the input
-- stream and rescan. Otherwise, just drop it into the output as is
-- and move on.
handlePERef :: SymTable -> IntSymTable -> L.Text -> Text -> [DTDComponent]
handlePERef ext int cont name = maybe moveOn rescan refVal
  where
    moveOn = DTDPERef name : parseCmps ext int cont
    rescan = (++ parseCmps ext int cont) .
             parseCmps ext intNoRecurse .
             L.concat . map (L.fromStrict . renderValue)
    refVal = join $ M.lookup name int
    intNoRecurse = M.insert name Nothing int

-- | Pre-parse a single DTD component at the beginning of the current
-- input text. Pre-parsing separates components that need parameter
-- entity replacement from those that do not.
preparse :: Parser PreParse
preparse = choice
    [ PPERef       <$> try pERef
    , PInstruction <$> try instruction
    , PComment     <$> try comment
    , PMarkup      <$> markup
    ]

-- | Pre-parse a markup declaration. We match only the opening and
-- closign pointy brackets, and break the text into quoted and
-- unquoted sub-texts. For quoted sub-texts, the quote marks are
-- dropped.
markup :: Parser [MarkupText]
markup = mkMarkup <$>
    ("<" .*> unquoted) <*>
    manyTillS ((:) . MTQuoted <$> try quoted <*> unquoted) ">"
  where
    unquoted = chunk2 <$> unqText <*> many' (list2 <$> pct <*> unqText)
    unqText = MTUnquoted <$> takeTill (inClass "%>'\"")
    pct = "%" .*> (MTUnquoted <$> (ws *> pure "% ") <|>
                   MTPERef <$> takeTill (== ';') <*. ";")
    mkMarkup ts tss = wrap . concat $ ts : tss
    wrap = (MTUnquoted "<" :) . (++ [MTUnquoted ">"])
    chunk2 x xss = x : concat xss

-- | To handle a pre-parsed component that is markup declaration, we
-- resolve parameter entities where necessary, parse it as a
-- component, and possibly update the internal symbol table.
handleMarkup :: SymTable -> IntSymTable -> L.Text -> [MarkupText] ->
                [DTDComponent]
handleMarkup ext int cont =
    handleCmp . parse dtdComponent . L.fromStrict . renderMarkup int
  where
    handleCmp (Done _ (DTDEntityDecl e)) = handleEntity ext int cont e
    handleCmp (Done _ cmp)               = cmp : parseCmps ext int cont
    handleCmp _                          = []

-- | Render pre-parsed markup as text, resolving parameter entities as
-- needed. Note that parameter entities inside quoted strings are not
-- resolved here; if later we recognize that the string is the value
-- being asigned to an entity in an entity declaration, we will
-- resolve the parameter then.
renderMarkup :: IntSymTable -> [MarkupText] -> Text
renderMarkup syms = T.concat . concatMap render
  where
    render (MTQuoted t)   = ["\"", t, "\""]
    render (MTUnquoted t) = [t]
    render (MTPERef  r)   = renderPERef syms r

-- | Finsh processing an entity declaration by resolving parameter
-- entity references in its value if it is internal, and updating the
-- internal symbol table if it is a parameter entity declaration. We
-- allow re-declaration of parameter entities, although that is
-- forbidden by the XML spec, but we ignore the new declaration when
-- resolving parameter entities.
handleEntity :: SymTable -> IntSymTable -> L.Text -> EntityDecl ->
                [DTDComponent]
handleEntity ext int cont e = DTDEntityDecl e' : parseCmps ext int' cont
  where
    (e', int') = case e of
      InternalGeneralEntityDecl   n val -> ige n val
      InternalParameterEntityDecl n val -> ipe n val
      ExternalParameterEntityDecl n _   -> epe n
      other                             -> (other, int)
    ige n v = (InternalGeneralEntityDecl n $ resolveValue int v, int)
    ipe n v = let v' = resolveValue int v
              in (InternalParameterEntityDecl n v', insertPE n v')
    epe n = (e, maybe int (insertPE n) $ resolveEPE n)
    insertPE n v = M.insertWith (const id) n (Just v) int
    resolveEPE n = fmap (resolveValue int) $
                   M.lookup n ext >>= maybeResult . parse parseEPE
    parseEPE = many' $
      EntityPERef <$> try pERef <|> EntityText <$> takeTill (== '%')

-- | Resolve nested parameter entity references in the value string
-- for an entity declaration.
resolveValue :: IntSymTable -> [EntityValue] -> [EntityValue]
resolveValue syms = concatMap combine . groupBy bothText . concatMap resolve
  where
    resolve e@(EntityPERef r) = fromMaybe [e] . join $ M.lookup r syms
    resolve e                 = [e]

    bothText (EntityText {}) (EntityText {}) = True
    bothText _               _               = False

    combine es@(EntityText {}:_) = [EntityText . T.concat . catMaybes $
                                    map justText es]
    combine es                   = es

    justText (EntityText t) = Just t
    justText _              = Nothing

-- | Render a parameter entity reference (which is not inside a quoted
-- value string in an entity declaration) as 'Text'. Render nested PEs
-- whose value is not known as textual PE refs. Insert a space before
-- and after the replacement text, as per the XML specification.
renderPERef :: IntSymTable -> PERef -> [Text]
renderPERef syms ref = maybe [pERefText ref] render . join $
                       M.lookup ref syms
  where
    render = (" " :) . (++ [" "]) . map renderValue

-- | Render an 'EntityValue' as 'Text', assuming that all known
-- parameter entity references have already been resolved.
renderValue :: EntityValue -> Text
renderValue (EntityText  t) = t
renderValue (EntityPERef r) = pERefText r

-- | Render an unresolved parameter entity as 'Text'.
pERefText :: PERef -> Text
pERefText r = T.concat ["%", r, ";"]

-- | Parse a DTD. Parameter entity substitution is not supported by
-- this parser, so parameter entities cannot appear in places where a
-- valid DTD syntax production cannot be determined without resolving
-- them.
dtd :: Parser DTD
dtd = DTD <$> (skipWS *> optional (textDecl <* skipWS)) <*>
      many' (dtdComponent <* skipWS)

-- | Parse an @?xml@ text declaration at the beginning of a 'DTD'.
textDecl :: Parser DTDTextDecl
textDecl = do
    "<?" .*> xml .*> ws *> skipWS
    enc1 <- optional $ try encoding
    ver  <- optional $ try (maybeSpace version enc1)
    enc  <- maybe (maybeSpace encoding ver) return enc1
    skipWS *> "?>" .*> pure (DTDTextDecl ver enc)
  where
    xml = ("X" <|> "x") .*> ("M" <|> "m") .*> ("L" <|> "l")
    version = attr "version" $ const versionNum
    versionNum = T.append <$> "1." <*> (T.singleton <$> digit)
    encoding = attr "encoding" $ takeTill . (==)
    attr name val = try (attrQ '"' name val) <|> attrQ '\'' name val
    attrQ q name val = name .*> skipWS *> "=" .*> skipWS *>
                       char q *> val q <* char q
    maybeSpace p = maybe p (const $ ws *> skipWS *> p)

-- | Parse a single component of a 'DTD'. Conditional sections are
-- currently not supported.
dtdComponent :: Parser DTDComponent
dtdComponent = choice $ map try
  [ DTDPERef       <$> pERef
  , DTDEntityDecl  <$> entityDecl
  , DTDElementDecl <$> elementDecl
  , DTDAttList     <$> attList
  , DTDNotation    <$> notation
  , DTDInstruction <$> instruction
  ] ++ -- no try needed for last choice
  [ DTDComment     <$> comment
  ]

-- | Parse a processing instruction.
instruction :: Parser Instruction
instruction = Instruction <$> ("<?" .*> skipWS *> nameSS) <*>
                              idata <*. "?>"
  where
    -- Break the content into chunks beginning with '?' so we
    -- can find the '?>' at the end. The first chunk might not
    -- begin with '?'.
    idata = T.concat . concat <$> manyTillS chunk "?>"
    chunk = list2 . T.singleton <$> anyChar <*> takeTill (== '?')

-- | Parse an entity declaration.
entityDecl :: Parser EntityDecl
entityDecl = "<!ENTITY" .*> ws *> skipWS *>
                choice [try internalParam, try externalParam,
                        try internalGen,   externalGen]
              <* skipWS <*. ">"
  where
    internalParam = InternalParameterEntityDecl <$>
                      (param *> nameSS) <*> entityValue
    externalParam = ExternalParameterEntityDecl <$>
                      (param *> nameSS) <*> externalID
    internalGen = InternalGeneralEntityDecl <$> nameSS <*> entityValue
    externalGen = ExternalGeneralEntityDecl <$>
                    nameSS <*> externalID <*> optional (try ndata)
    param = "%" .*> ws *> skipWS
    ndata = skipWS *> "NDATA" .*> ws *> skipWS *> name

-- | Parse a DTD name. We are much more liberal than the spec: we
-- allow any characters that will not interfere with other DTD
-- syntax. This parser subsumes both @Name@ and @NmToken@ in the spec,
-- and more.
name :: Parser Text
name = nonNull $ takeTill notNameChar
  where
    notNameChar c = isSpace c || inClass syntaxChars c
    syntaxChars = "()[]<>!%&;'\"?*+|,="
    nonNull parser = do
      text <- parser
      guard . not . T.null $ text
      return text

-- | Parse a DTD 'name' followed by optional white space.
nameSS :: Parser Text
nameSS = name <* skipWS

-- | Parse an entity value. An entity value is a quoted string
-- possibly containing parameter entity references.
entityValue :: Parser [EntityValue]
entityValue = try (quotedVal '"') <|> quotedVal '\''
  where
    quotedVal q = char q *> manyTill (content q) (char q)
    content q =  EntityPERef <$> try pERef <|> EntityText <$> text q
    text q = takeTill $ \c -> c == '%' || c == q

-- | Parse a parameter entity reference
pERef :: Parser PERef
pERef = "%" .*> name <*. ";"

-- | Parse the declaration of an element.
elementDecl :: Parser ElementDecl
elementDecl = ElementDecl <$> ("<!ELEMENT" .*> ws *> skipWS *> nameSS) <*>
                              contentDecl <* skipWS <*. ">"

-- | Parse the content that can occur in an element.
contentDecl :: Parser ContentDecl
contentDecl = choice $ map try
    [ pure ContentEmpty <*. "EMPTY"
    , pure ContentAny   <*. "ANY"
    ,      ContentMixed <$> pcdata
    ] ++
    [      ContentElement <$> contentModel
    ]
  where
    pcdata = "(" .*> skipWS *> "#PCDATA" .*> skipWS *>
             (try tags <|> noTagsNoStar)
    tags = many' ("|" .*> skipWS *> nameSS) <*. ")*"
    noTagsNoStar = ")" .*> pure []

-- | Parse the model of structured content for an element.
contentModel = choice $ map (<*> repeatChar)
    [ CMChoice <$> try (cmList '|')
    , CMSeq    <$> try (cmList ',')
    , CMName   <$> name
    ]
  where
    cmList sep = "(" .*> skipWS *>
      ((contentModel <* skipWS) `sepBy1` (char sep *> skipWS)) <*. ")"

-- | Parse a repetition character.
repeatChar :: Parser Repeat
repeatChar = choice
  [ char '?' *> pure ZeroOrOne
  , char '*' *> pure ZeroOrMore
  , char '+' *> pure OneOrMore
  ,             pure One
  ]

-- | Parse a list of attribute declarations for an element.
attList :: Parser AttList
attList = AttList <$> ("<!ATTLIST" .*> ws *> skipWS *> nameSS) <*>
                       many' attDecl <*. ">"

-- | Parse the three-part declaration of an attribute.
attDecl :: Parser AttDecl
attDecl = AttDecl <$>
           nameSS <*> attType <* skipWS <*> attDefault <* skipWS

-- | Parse the type of an attribute.
attType :: Parser AttType
attType = choice $ map try
    -- The ws is required by the spec, and needed by the parser to be
    -- able to distinguish between ID and IDREF, and NMTOKEN and
    -- NMTOKENS.
    [ "CDATA"    .*> ws *> pure AttStringType
    , "ID"       .*> ws *> pure AttIDType
    , "IDREF"    .*> ws *> pure AttIDRefType
    , "IDREFS"   .*> ws *> pure AttIDRefsType
    , "ENTITY"   .*> ws *> pure AttEntityType
    , "ENTITIES" .*> ws *> pure AttEntitiesType
    , "NMTOKEN"  .*> ws *> pure AttNmTokenType
    , "NMTOKENS" .*> ws *> pure AttNmTokensType
    ,  AttEnumType <$> enumType
    ] ++
    [ AttNotationType <$> notationType
    ]
  where
    enumType = nameList
    notationType = "NOTATION" .*> ws *> skipWS *> nameList
    nameList = "(" .*> skipWS *>
               (nameSS `sepBy1` ("|" .*> skipWS)) <*. ")"

-- | Parse a default value specification for an attribute.
attDefault :: Parser AttDefault
attDefault = choice $ map try
    [ "#REQUIRED" .*> pure AttRequired
    , "#IMPLIED"  .*> pure AttImplied
    , AttFixed <$> ("#FIXED" .*> ws *> skipWS *> quoted)
    ] ++
    [ AttDefaultValue <$> quoted
    ]

-- | A single-quoted or double-quoted string. The quotation marks are
-- dropped.
quoted :: Parser Text
quoted = quotedWith '"' <|> quotedWith '\''
  where
    quotedWith q = char q *> takeTill (== q) <* char q

-- | Parse a declaration of a notation.
notation :: Parser Notation
notation = Notation <$>
  ("<!NOTATION" .*> ws *> skipWS *> name) <* ws <* skipWS <*>
  notationSrc <*. ">"

-- | Parse a source for a notation.
notationSrc :: Parser NotationSource
notationSrc = try system <|> public
  where
    system = NotationSysID <$>
      ("SYSTEM" .*> ws *> skipWS *> quoted <* ws <* skipWS)
    public = mkPublic <$>
      ("PUBLIC" .*> ws *> skipWS *> quoted) <*>
      optional (try $ ws *> skipWS *> quoted) <* skipWS
    mkPublic pubID = maybe (NotationPubID pubID) (NotationPubSysID pubID)

-- | Parse an external ID.
externalID :: Parser ExternalID
externalID = try system <|> public
  where
    system = SystemID <$> ("SYSTEM" .*> ws *> skipWS *> quoted)
    public = PublicID <$> ("PUBLIC" .*> ws *> skipWS *> quoted) <*
                          ws <* skipWS <*> quoted

-- | Parse a comment
comment :: Parser Text
comment = "<!--" .*> (T.concat . concat <$> manyTillS chunk "--") <*. ">"
  where
    chunk = list2 . T.singleton <$> anyChar <*> takeTill (== '-')

-- | Definition of white space characters, from the XML specification.
isXMLSpace :: Char -> Bool
isXMLSpace = inClass "\x20\x9\xD\xA"

-- | Parse one character of white space.
ws :: Parser Char
ws = satisfy isXMLSpace

-- | Skip zero or more characters of white space
skipWS :: Parser ()
skipWS = A.takeWhile isXMLSpace *> pure ()

-- | Type-specialized version of manyTill, so we can use the 'IsString'
-- instance for 'Parser' 'Text' with it.
manyTillS :: Parser a -> Parser Text -> Parser [a]
manyTillS = manyTill

-- | Create a two-element list.
list2 :: a -> a -> [a]
list2 x y = [x, y]

-- This is lifted from attoparsec-text library.
-- | Same as @Applicative@'s @\<*@ but specialized to 'Text'
-- on the second argument.
(<*.) :: Applicative f => f a -> f Text -> f a
(<*.) = (<*)

-- This is lifted from attoparsec-text library.
-- | Same as @Applicative@'s @*\>@ but specialized to 'Text'
-- on the first argument.
(.*>) :: Applicative f => f Text -> f a -> f a
(.*>) = (*>)
