#include "Node.h"

namespace MyLib {

	std::string Node::TypeToStr() const {
		switch (this->GetType()) {
		case TypeOfNode::a_node:	   return "a_node";
		case TypeOfNode::plus_node:    return "plus_node";
		case TypeOfNode::and_node:     return "and_node";
		case TypeOfNode::or_node:      return "or_node";
		case TypeOfNode::capture_node: return "capture_node";
		default:					   return "";
		};
	}

	std::ostream& operator<< (std::ostream& out, const Node& n) {

		out << n.TypeToStr() << " ( ";

		switch (n.nullable) {
		case false:
			out << "-";
			break;
		case true:
			out << "+";
			break;
		};

		out << " {" << n.GetVal().first << "}  " << n.GetVal().second << " )";

		return out;
	}
}