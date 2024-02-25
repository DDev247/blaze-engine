
#include "../../include/blaze_html.h"

#include <iostream>
#include <fstream>

int main(int argc, char** argv) {

    std::cout << "Blaze " << BLAZE_VERSION << std::endl;
    std::cout << "HTML test" << std::endl;

    /* ------------------------- Test hierarchy creation ------------------------ */

    blaze::html::Document document;
    blaze::html::Element head("head");
    blaze::html::Element title("title");
    title.textContent = "Test Document";

    head.children.push_back(&title);
    document.children.push_back(&head);
    // document.children.push_back(body);

    std::cout << document.GetTree();

    /* -------------------------- Test document parsing ------------------------- */

    blaze::html::Document parsedDocument;
    std::string docString("");

    // Read test file
    std::ifstream file("test.html");
    std::string line;
    if(!file.is_open()) {
        std::cerr << "Failed to open test.html";
        return 1;
    }

    while(std::getline(file,line))
        docString += line + "\n";

    file.close();

    parsedDocument = blaze::html::DocumentParser::parseStringUnsafe(docString);
    std::cout << parsedDocument.GetTree();

    return 0;
}
