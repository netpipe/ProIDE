// Project ID: 30271439
// Codeblocks makefile generator
// would like a app wrote with qt or libxml to convert .cbp file to a makefile
//

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <cstring>
#include <map>
#include <iterator>

#include <libxml/parser.h>

struct Config
{
	std::string compiler;
	std::vector<std::string> compilerOptions;	
	std::vector<std::string> compilerDirectories;
	std::vector<std::string> cppFiles;
	std::vector<std::string> otherFiles;
	std::string compilerOutput;
	
	std::vector<std::string> resourceOptions;
	std::vector<std::string> resourceDirectories;
	std::string resourceOutput;


	std::string linker;
	std::vector<std::string> linkerOptions;
	std::vector<std::string> linkerLibraries;
	std::vector<std::string> linkerDirectories;
	std::string linkerOutput;
};

std::vector<std::string> targets;
std::map<std::string, Config> configs;

void HandleTarget(xmlNode * targetNode, bool global)
{
	std::string target = "";
	if (!global)
	{
		target = (const char*)xmlGetProp(targetNode, (const xmlChar*)"title");
		targets.push_back(target);
		configs[target] = Config();
	}

	xmlNode * node1 = targetNode->children;
	while (node1)
	{
		if (node1->type == XML_ELEMENT_NODE && !strcmp((const char*)node1->name, "Option"))
		{
			if (xmlHasProp(node1, (const xmlChar*)"object_output"))
			{				
				std::string output_option = (const char*)xmlGetProp(node1, (const xmlChar*)"object_output");				
				configs[target].compilerOutput = output_option;				
			}
			if (xmlHasProp(node1, (const xmlChar*)"output"))
			{
				std::string output_option = (const char*)xmlGetProp(node1, (const xmlChar*)"output");
				configs[target].linkerOutput = output_option;
			}
			if (xmlHasProp(node1, (const xmlChar*)"compiler"))
			{
				std::string compiler = (const char*)xmlGetProp(node1, (const xmlChar*)"compiler");				
				configs[target].compiler = compiler;
			}
		}

		if (node1->type == XML_ELEMENT_NODE && !strcmp((const char*)node1->name, "Compiler"))
		{
			xmlNode * node2 = node1->children;
			while (node2)
			{
				if (node2->type == XML_ELEMENT_NODE && !strcmp((const char*)node2->name, "Add"))
				{
					if (xmlHasProp(node2, (const xmlChar*)"option"))
					{
						std::string option = (const char*)xmlGetProp(node2, (const xmlChar*)"option");
						configs[target].compilerOptions.push_back(option);
					}
					if (xmlHasProp(node2, (const xmlChar*)"directory"))
					{
						std::string directory = (const char*)xmlGetProp(node2, (const xmlChar*)"directory");
						configs[target].compilerDirectories.push_back(directory);
					}
				}
				node2 = node2->next;
			}
		}
		if (node1->type == XML_ELEMENT_NODE && !strcmp((const char*)node1->name, "Linker"))
		{
			xmlNode * node2 = node1->children;
			while (node2)
			{
				if (node2->type == XML_ELEMENT_NODE && !strcmp((const char*)node2->name, "Add"))
				{
					if (xmlHasProp(node2, (const xmlChar*)"option"))
					{
						std::string option = (const char*)xmlGetProp(node2, (const xmlChar*)"option");
						configs[target].linkerOptions.push_back(option);
					}
					if (xmlHasProp(node2, (const xmlChar*)"directory"))
					{
						std::string directory = (const char*)xmlGetProp(node2, (const xmlChar*)"directory");
						configs[target].linkerDirectories.push_back(directory);
					}
					if (xmlHasProp(node2, (const xmlChar*)"library"))
					{
						std::string library = (const char*)xmlGetProp(node2, (const xmlChar*)"library");
						configs[target].linkerLibraries.push_back(library);
					}
				}
				node2 = node2->next;
			}
		}
		if (node1->type == XML_ELEMENT_NODE && !strcmp((const char*)node1->name, "Unit"))
		{			
			std::string filename = (const char*)xmlGetProp(node1, (const xmlChar*)"filename");
			if (filename.substr(filename.size()-4)==".cpp" || filename.substr(filename.size() - 2) == ".c")
				configs[target].cppFiles.push_back(filename);
			else
				configs[target].otherFiles.push_back(filename);
		}
		node1 = node1->next;
	}		
}

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
	if (from.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

std::string& implode(const std::vector<std::string>& elems, std::string delim, std::string& s, std::string prefix)
{
	for (std::vector<std::string>::const_iterator ii = elems.begin(); ii != elems.end(); ++ii)
	{
		s += prefix + (*ii);
		if (ii + 1 != elems.end()) {
			s += delim;
		}
	}

	return s;
}

std::string implode(const std::vector<std::string>& elems, std::string delim, std::string prefix)
{
	std::string s;
	return implode(elems, delim, s, prefix);
}

std::string getObjectFileName(std::string output, std::string cppFile)
{
	std::string objectfilename = "";
	if (output.size() > 0)
	{
		objectfilename = output;
		if (objectfilename[objectfilename.size() - 1]!='\\' && objectfilename[objectfilename.size() - 1] != '/')
			objectfilename += "\\";
	}

	objectfilename += cppFile.substr(0, cppFile.size() - 4) + ".o";
	return objectfilename;
}

std::string parent(std::string dir)
{
	std::string parent_dir = "";
	int p1 = (int)dir.find_last_of('/');
	int p2 = (int)dir.find_last_of('\\');
	int p3 = std::max(p1, p2);
	if (p3 > 0)
		parent_dir = dir.substr(0, p3);

	return parent_dir;
}

int main(int argc, char* argv[])
{
	xmlDoc *doc = NULL;
	xmlNode *root_element = NULL;

	doc = xmlReadFile((const char*)argv[1], NULL, 0);

	xmlNode * node1 = xmlDocGetRootElement(doc);

	while (node1)
	{
		if (node1->type == XML_ELEMENT_NODE && !strcmp((const char*)node1->name, "CodeBlocks_project_file"))
		{
			xmlNode * node2 = node1->children;
			while (node2)
			{
				if (node2->type == XML_ELEMENT_NODE && !strcmp((const char*)node2->name, "Project"))
				{
					xmlNode * node3 = node2->children;
					while (node3)
					{
						if (node3->type == XML_ELEMENT_NODE && !strcmp((const char*)node3->name, "Build"))
						{
							xmlNode * node4 = node3->children;
							while (node4)
							{
								if (node4->type == XML_ELEMENT_NODE && !strcmp((const char*)node4->name, "Target"))
								{
									HandleTarget(node4, false);
								}
								node4 = node4->next;
							}
						}
						node3 = node3->next;
					}
					HandleTarget(node2, true);
				}
				node2 = node2->next;
			}
		}
		//
		node1 = node1->next;
	}

	xmlFreeDoc(doc);
	xmlCleanupParser();

	std::ofstream makefile("makefile");
	if (makefile)
	{
		for (size_t i = 0; i < targets.size(); i++)
		{
			std::string target = targets[i];
			std::string written_target = targets[i];
			replaceAll(written_target, " ", "\\ ");
			makefile << written_target << ":" << std::endl;

			std::string outDir = configs[target].compilerOutput;
			if (outDir.size() != 0)
			{
				makefile << "\tmkdir -p \"" << outDir << "\"" << std::endl;
			}

			std::vector<std::string> specCppFiles = configs[target].cppFiles;
			std::vector<std::string> genericCppFiles = configs[""].cppFiles;

			std::vector<std::string> allCppFiles;
			allCppFiles.insert(allCppFiles.end(), std::make_move_iterator(specCppFiles.begin()), std::make_move_iterator(specCppFiles.end()));
			allCppFiles.insert(allCppFiles.end(), std::make_move_iterator(genericCppFiles.begin()), std::make_move_iterator(genericCppFiles.end()));

			std::vector<std::string> specCompilerOptions = configs[target].compilerOptions;
			std::vector<std::string> genericCompilerOptions = configs[""].compilerOptions;

			std::vector<std::string> allCompilerOptions;
			allCompilerOptions.insert(allCompilerOptions.end(), std::make_move_iterator(specCompilerOptions.begin()), std::make_move_iterator(specCompilerOptions.end()));
			allCompilerOptions.insert(allCompilerOptions.end(), std::make_move_iterator(genericCompilerOptions.begin()), std::make_move_iterator(genericCompilerOptions.end()));

			std::vector<std::string> specCompilerDirectories = configs[target].compilerDirectories;
			std::vector<std::string> genericCompilerDirectories = configs[""].compilerDirectories;

			std::vector<std::string> allCompilerDirectories;
			allCompilerDirectories.insert(allCompilerDirectories.end(), std::make_move_iterator(specCompilerDirectories.begin()), std::make_move_iterator(specCompilerDirectories.end()));
			allCompilerDirectories.insert(allCompilerDirectories.end(), std::make_move_iterator(genericCompilerDirectories.begin()), std::make_move_iterator(genericCompilerDirectories.end()));

			std::vector<std::string> specLinkerOptions = configs[target].linkerOptions;
			std::vector<std::string> genericLinkerOptions = configs[""].linkerOptions;

			std::vector<std::string> allLinkerOptions;
			allLinkerOptions.insert(allLinkerOptions.end(), std::make_move_iterator(specLinkerOptions.begin()), std::make_move_iterator(specLinkerOptions.end()));
			allLinkerOptions.insert(allLinkerOptions.end(), std::make_move_iterator(genericLinkerOptions.begin()), std::make_move_iterator(genericLinkerOptions.end()));

			std::vector<std::string> specLinkerDirectories = configs[target].linkerDirectories;
			std::vector<std::string> genericLinkerDirectories = configs[""].linkerDirectories;

			std::vector<std::string> allLinkerDirectories;
			allLinkerDirectories.insert(allLinkerDirectories.end(), std::make_move_iterator(specLinkerDirectories.begin()), std::make_move_iterator(specLinkerDirectories.end()));
			allLinkerDirectories.insert(allLinkerDirectories.end(), std::make_move_iterator(genericLinkerDirectories.begin()), std::make_move_iterator(genericLinkerDirectories.end()));


			std::vector<std::string> specLinkerLibraries = configs[target].linkerLibraries;
			std::vector<std::string> genericLinkerLibraries = configs[""].linkerLibraries;

			std::vector<std::string> allLibraries;
			allLibraries.insert(allLibraries.end(), std::make_move_iterator(specLinkerLibraries.begin()), std::make_move_iterator(specLinkerLibraries.end()));
			allLibraries.insert(allLibraries.end(), std::make_move_iterator(genericLinkerLibraries.begin()), std::make_move_iterator(genericLinkerLibraries.end()));

			std::vector<std::string> allObjFiles;
			for (size_t j = 0; j < allCppFiles.size(); j++)
			{
				allObjFiles.push_back("\"" + getObjectFileName(outDir, allCppFiles[j]) + "\"");
				makefile << "\t" << "g++" << " " << implode(allCompilerOptions, " ", "") << " " << implode(allCompilerDirectories, " ", "-I") << " -c \"" << allCppFiles[j] << "\" -o \"" << getObjectFileName(outDir, allCppFiles[j]) << "\"" << std::endl;
			}
			
			outDir = configs[target].linkerOutput;
			outDir = parent(outDir);

			if (outDir.size() != 0)
			{
				makefile << "\tmkdir -p \"" << outDir << "\"" << std::endl;
			}
			
			std::string exeFileName = configs[target].linkerOutput + ".exe";

			makefile << "\t" << "g++" << " " << implode(allLinkerOptions, " ", "") << " " << implode(allLinkerDirectories, " ", "-L") << " -o \"" << exeFileName << "\" " << implode(allObjFiles, " ", "") << " " << implode(allLibraries, " ", "-l") << std::endl;

			makefile << std::endl;
		}
		makefile.close();
	}

	return 1;
}
