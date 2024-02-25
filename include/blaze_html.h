#ifndef A7F2B943_C1FF_4F5E_992A_870788D7E5F3
#define A7F2B943_C1FF_4F5E_992A_870788D7E5F3

#include <vector>
#include <string>
#include <map>

#define CREATE_ELEMENT(arg1, arg2) \
class arg1##Element : public Element { \
public: \
    arg1##Element() : Element(#arg2) { this->name = #arg2; } \
}

namespace blaze::html {

    class Element {
    public:
        /// @brief Create a new Element
        /// @param n Element name
        Element(std::string n) { this->name = n; }
        
        /// @brief Element children
        std::vector<Element*> children;

        /// @brief Element attributes as strings
        std::map<std::string, std::string> attributes;

        /// @brief Text content of the element (WARNING: does not include children text)
        std::string textContent;

        /// @brief Get the Element's name
        virtual inline std::string GetName() { return name; }

        /// @brief Set the Element's name
        /// @param n String to set the name to
        inline void SetName(std::string n) { name = n; }
        
        /// @brief Get tree visualisation of the element
        /// @param depth Maximum depth (default 5)
        /// @return Tree visualisation of the element
        std::string GetTree(int depth = 5);

        /// @brief Parent of the element
        Element* parent;

    protected:
        std::string GetTreeRecursive(int depth, int initialDepth);
        std::string name = "";
    };

    // CREATE_ELEMENT(Head, head);
    // CREATE_ELEMENT(Body, body);
    // CREATE_ELEMENT(Title, title);

    class Document : public Element {
    public:
        Document() : Element("!doc") { this->name = "!doc"; }
    };

    /// @brief Token for the DocumentParser
    struct DocumentParserToken {
        enum class Type {
            String, // a-zA-Z
            Space, // ' '
            Setter, // =
            Value, // "

            TagOpen, // <
            TagClose, // >
            TagOpenSlash, // </

			CommentStart, // !--
			CommentEnd, // --

			Newline, // \n

			Unknown, // others

		} type;

        /// @brief Constructs a new DocumentParserToken
        /// @param t Token Type
        /// @param s Token String (content)
        DocumentParserToken(Type t, std::string s) { this->type = t; this->s = s;}

        /// @brief Gives you the Type as a string
        /// @return Type name
        std::string TypeToString() {
            switch (this->type)
            {
            case Type::String:
                return "String";
            case Type::Setter:
                return "Setter";
            case Type::Value:
                return "Value";
            case Type::TagOpen:
                return "TagOpen";
            case Type::TagClose:
                return "TagClose";
            case Type::TagOpenSlash:
                return "TagOpenSlash";
            case Type::CommentStart:
                return "CommentStart";
            case Type::CommentEnd:
                return "CommentEnd";
            case Type::Newline:
                return "Newline";
            case Type::Space:
                return "Space";

            default:
                return "?";
            }
        }

        std::string s;
    };

    /// @brief Document parser
    struct DocumentParser {
        /// @brief Parses a Document from a HTML string
        /// @param docString HTML string
        /// @return Result Document
        static Document parseStringUnsafe(std::string docString);
        
        /// @brief Regex map for tokens
        static std::map<std::string, DocumentParserToken::Type> tokenMap;
    
    private:
        
        /// @brief Tokenizes the HTML string into a list of DocumentParserToken
        /// @param docString HTML String
        /// @return List of Tokens
        static std::vector<DocumentParserToken> tokenize(std::string docString);
        
        /// @brief Get child of Document by child depth list
        /// @param doc Document to iterate
        /// @param depth List of child depths (for example [0, 1] will get child 1 of child 0 of document)
        static Element* getChildByDepth(Document doc, std::vector<int> depth);
    };
};

#endif /* A7F2B943_C1FF_4F5E_992A_870788D7E5F3 */
