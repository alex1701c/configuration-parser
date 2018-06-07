#include "dump_common.h"
#include "string-helpers.h"

#include <QList>
#include <string>

void dump_source_get_methods(std::ofstream& f, MetaClass *top) 
{
  // get - methods.
  for (auto &&p : top->properties) {
    f << p->type << ' ' << top->name << "::" << p->name << "() const"
         << std::endl;
    f << '{' << std::endl;
    f << "\treturn _" << p->name << ';' << std::endl;
    f << '}' << std::endl;
    f << std::endl;
  }
}

void dump_source_set_methods(std::ofstream& f, MetaClass *top) {
  for (auto &&p : top->properties) {
    f << "void " << top->name << "::set" << capitalize(p->name, 0) << '(';
        dump_parameter(f, p);
    f << ")" << std::endl;
    f << "{" << std::endl;
    f << "\tif (_" << p->name << "==" << "value) {" << std::endl;
    f << "\t\treturn;" << std::endl;
    f << "\t}" << std::endl;
    f << "\t _" << p->name << " = value;" << std::endl;
    f << "\temit " << p->name << "Changed(value);" << std::endl;
    f << "}" << std::endl;
    f << std::endl;
  }
}

void dump_parameter(std::ofstream& file, const std::shared_ptr<MetaProperty>& p)
{
    if (pass_as_const_ref(p->type)) {
        file << "const " << p->type << "&";
    } else {
        file << p->type;
    }

    file << " value";
}

void dump_header_properties(std::ofstream &file, const std::vector<std::shared_ptr<MetaProperty>> &properties) {
  if (!properties.size())
    return;

  for (auto &&p : properties) {
    file << "\t" << p->type << " " << p->name << "() const;" << std::endl;
  }

  file << std::endl << "public slots:" << std::endl;
  for (auto &&p : properties) {
    file << "\tvoid set" << capitalize(p->name, 0) << "(";
        dump_parameter(file, p);
    file << ");" << std::endl;
  }
  
  file << std::endl << "signals:" << std::endl;
  for (auto &&p : properties) {
    file << "\tvoid " << p->name << "Changed(";
        dump_parameter(file, p);
    file << ");" << std::endl;
  }

  file << std::endl << "private:" << std::endl;
  for (auto &&p : properties) {
    file << "\t" << p->type << " _" << p->name << ";" << std::endl;
  }
}

void dump_header_q_properties(std::ofstream &file, const std::vector<std::shared_ptr<MetaProperty>> &properties)
{
    for (auto &&p : properties) {
    file << "\tQ_PROPERTY(" << p->type << " "
         << camel_case_to_underscore(p->name) << " READ " << p->name
         << " WRITE set" << capitalize(p->name, 0) << " NOTIFY " << p->name
         << "Changed)" << std::endl;
  }
}

bool pass_as_const_ref(const std::string& type) {
    for(const auto simple_type : {"int", "bool", "char", "double", "float", "qreal", "size_t", "QChar"}) {
        if (type == simple_type) {
            return false;
        }
    }
    // This is probably false but will have a ok rate of false positives.
    return true;
}