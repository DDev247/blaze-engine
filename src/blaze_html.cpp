
#include "../include/blaze_html.h"
#include <iostream> // For debugging
#include <regex>
using namespace blaze::html;

std::string Element::GetTreeRecursive(int depth, int initialDepth) {
    if(depth < 0) return "\n";

    std::string result = "";
    for (int i = 0; i <= initialDepth - depth; i++)
        result += "  ";
    result += "\\> " + this->GetName() + " - \"" + this->textContent + "\"";
    if(this->attributes.size() > 0) {
        result += " -";

        for(auto attr : attributes) {
            result += " [\"" + attr.first + "\"]: \"" + attr.second + "\"";
        }

        result += "\n";
    }
    else result += "\n";

    for(Element* e : children) {
        result += e->GetTreeRecursive(depth - 1, initialDepth);
    }

    return result;
}

std::string Element::GetTree(int depth) {
    std::string result = "";
    result += this->GetName() + " - \"" + this->textContent + "\"\n";

    for(Element* e : children) {
        result += e->GetTreeRecursive(depth, depth);
    }

    return result;
}

Element* DocumentParser::getChildByDepth(Document doc, std::vector<int> depth) {
    Element* r;
    for(int i = 0; i < depth.capacity(); i++) {
        int d = depth[i];
        if(i == 0) {
            r = doc.children[d];
        }
        else {
            r = r->children[d];
        }
    }
    return r;
}

Document DocumentParser::parseStringUnsafe(std::string docString) {
    Document result;
    
    /* ----------------------------- Tokenize string ---------------------------- */
    std::vector<DocumentParserToken> tokens = tokenize(docString);

    /* -------------------- Parse tokens & generate elements -------------------- */
    DocumentParserToken tkLast(DocumentParserToken::Type::Unknown, "");
    Element* elNew;
    std::vector<int> depth = { };
    bool insideElement = false;
    bool closingElement = false;
    bool comment = false;

    // attributes
    bool insideElementTag = false;
    bool insideAttribute = false;
    bool wantAttributeSetter = false;
    std::string attributeName;
    std::string attributeValue;

    // fuck you c++
    Element* e;
    std::string n;
    for(DocumentParserToken tk : tokens) {
        std::cout << (int)tk.type << "-" << tk.TypeToString() << " - " << tk.s << std::endl;
        if(tk.type == DocumentParserToken::Type::Newline) continue; // Skip newlines

        switch (tk.type)
        {
        case DocumentParserToken::Type::String:
            if(comment) break;
            if(elNew->GetName() == "?") {
                std::cout << "new element " << tk.s << std::endl;
                
                std::string data = tk.s;
                std::transform(data.begin(), data.end(), data.begin(),
                    [](unsigned char c){ return std::tolower(c); });

                elNew->SetName(data);
                elNew->parent = getChildByDepth(result, depth);
                elNew->parent->children.push_back(elNew);
                depth.push_back(elNew->parent->children.capacity() - 1);
                insideElementTag = true;
                insideAttribute = false;
                wantAttributeSetter = false;
                attributeName = "";
                attributeValue = "";
            }
            else if(insideElement) {
                getChildByDepth(result, depth)->textContent += tk.s;
            }
            else if(closingElement) {
                Element* e = getChildByDepth(result, depth);
                if(tk.s.compare(e->GetName()) != 0) {
                    // TODO: Implement some sort of error here?
                    // std::cout << "wrong closing tag name? got " << tk.s << ", expected " << e->GetName() << std::endl;
                }
            }
            else if(insideElementTag) {
                if(!insideAttribute) {
                    attributeName += tk.s;
                    wantAttributeSetter = true;
                }
                else {
                    attributeValue += tk.s;
                }
            }
            break;
        case DocumentParserToken::Type::Space:
            if(comment) break;
            if(insideElement) {
                getChildByDepth(result, depth)->textContent += tk.s;
            }
            else if(insideAttribute) {
                attributeValue += tk.s;
            }
            break;
        case DocumentParserToken::Type::Setter:
            if(comment) break;
            if(wantAttributeSetter) {
                // ?
            }
            else if(insideElement) {
                getChildByDepth(result, depth)->textContent += tk.s;
            }
            break;
        case DocumentParserToken::Type::Value:
            if(comment) break;
            if(wantAttributeSetter) {
                insideAttribute = true;
                wantAttributeSetter = false;                
            }
            else if(insideAttribute) {
                insideAttribute = false;
                getChildByDepth(result, depth)->attributes[attributeName] = attributeValue;
                attributeName = "";
                attributeValue = "";
            }
            else if(insideElement) {
                getChildByDepth(result, depth)->textContent += tk.s;
            }
            break;
        case DocumentParserToken::Type::TagOpen:
            elNew = new Element("?");
            break;
        case DocumentParserToken::Type::TagClose:
            //selfclosing tags
            e = getChildByDepth(result, depth);
            n = e->GetName();
            if(n == "!doctype" ||
                n == "img") {
                closingElement = true;
            }

            if(insideElementTag) insideElementTag = false;
            if(!closingElement) insideElement = true;
            if(closingElement) {
                closingElement = false;
                depth.pop_back();
            }
            break;
        case DocumentParserToken::Type::TagOpenSlash:
            // std::cout << "closing element " << getChildByDepth(result, currentElementDepth)->GetName() << std::endl;
            closingElement = true;
            insideElement = false;
            break;
        case DocumentParserToken::Type::CommentStart:
            comment = true;
            elNew->SetName("no");
            break;
        case DocumentParserToken::Type::CommentEnd:
            comment = false;
            break;

        default:
            break;
        }
    }

    // return result
    return result;
}

std::vector<DocumentParserToken> DocumentParser::tokenize(std::string docString) {
    std::vector<DocumentParserToken> result;
    
    for (int i = 0; i < docString.length(); i++) {
		std::string s = docString.substr(i);
		std::smatch res;

		bool matched = false;

		for (auto element : tokenMap) {
			std::regex reg(element.first);

			if (std::regex_search(s, res, reg)) {
				if (element.second == DocumentParserToken::Type::Newline)
			        result.push_back(DocumentParserToken(element.second, ""));
				else
			        result.push_back(DocumentParserToken(element.second, docString.substr(i, res.length())));

				i += res.length()-1;
				matched = true;
				break;
			}
		}

		if (!matched) {
			result.push_back(DocumentParserToken(DocumentParserToken::Type::Unknown, docString.substr(i, 1)));
		}
	}

    return result;
}

std::map<std::string, DocumentParserToken::Type> DocumentParser::tokenMap = {
	{ "^[a-zA-Z0-9\\!\\?\\:\\;]+", DocumentParserToken::Type::String},
	{ "^[ ]+", DocumentParserToken::Type::Space},
	{ "^(=)", DocumentParserToken::Type::Setter},
	{ "^\\\"", DocumentParserToken::Type::Value},

	{ "^(\\<\\/)", DocumentParserToken::Type::TagOpenSlash},
	{ "^\\<", DocumentParserToken::Type::TagOpen},
	{ "^\\>", DocumentParserToken::Type::TagClose},

	{ "^(!--)", DocumentParserToken::Type::CommentStart},
	{ "^(--)", DocumentParserToken::Type::CommentEnd},

	{ "^[\\\r\\\n]", DocumentParserToken::Type::Newline},
};
