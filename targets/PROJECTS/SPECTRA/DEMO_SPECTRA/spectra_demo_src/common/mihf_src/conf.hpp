//=======================================================================================================
// Brief   : Configuration DSL
// Authors : Bruno Santos <bsantos@av.it.pt>
// ------------------------------------------------------------------------------------------------------
// ODTONE - Open Dot Twenty One
//
// Copyright (C) 2009-2013 Universidade Aveiro
// Copyright (C) 2009-2013 Instituto de Telecomunicações - Pólo Aveiro
//
// This software is distributed under a license. The full license
// agreement can be found in the file LICENSE in this distribution.
// This software may not be copied, modified, sold or distributed
// other than expressed in the named license agreement.
//
// This software is distributed without any warranty.
//=======================================================================================================

#ifndef ODTONE_PMIP_CONF__HPP_
#define ODTONE_PMIP_CONF__HPP_

/////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <odtone/base.hpp>
#include <odtone/net/ip/address.hpp>
#include <boost/variant.hpp>
#include <boost/function.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/back.hpp>
#include <boost/mpl/deref.hpp>
#include <boost/mpl/prior.hpp>
#include <boost/function_types/result_type.hpp>
#include <boost/function_types/parameter_types.hpp>
#include <boost/function_types/function_arity.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/mpl/find.hpp>
#include <algorithm>
#include <cstring>
#include <string>
#include <map>

/////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace odtone { namespace conf {

namespace qi = boost::spirit::qi;
namespace ph = boost::phoenix;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
enum error_reason {
	invalid_syntax,
	invalid_command,
	invalid_arg_type,
	to_many_args,
	invalid_prop,
	invalid_prop_arg_type,
	to_many_prop_args,

	error_reason_size
};

static char const* const error_reason_string[error_reason_size] = {
	"invalid syntax",
	"invalid command",
	"invalid argument type",
	"to many arguments",
	"invalid property",
	"invalid property argument type",
	"to many property arguments"
};

typedef boost::variant<uint,
                       sint,
                       double,
                       std::string,
                       net::ip::address_v4,
                       net::ip::address_v6
                      > arg_type;

typedef std::vector<arg_type>             args_type;
typedef std::pair<std::string, args_type> prop_type;
typedef std::map<std::string, args_type>  pset_type;

template<class T>
class type_id_ {
	typedef typename boost::remove_cv<typename boost::remove_reference<T>::type>::type        tt;
	typedef typename boost::mpl::find<arg_type::types, tt>::type                              ft;
	typedef typename boost::mpl::distance<boost::mpl::begin<arg_type::types>::type, ft>::type tp;

	ODTONE_STATIC_ASSERT(
		(!boost::is_same<typename boost::mpl::end<arg_type::types>::type, ft>::value),
		"Type not supported"
	);

public:
	static const uint value = tp::value;
};

struct property_class {
	const char* name;    //property name
	uint        type_id; //property argument type
	uint        count;   //maximum number of arguments it can take
};

class function {
	template<class F, size_t N>
	class at_ {
		typedef typename boost::function_types::parameter_types<F>::type ps;
		typedef typename boost::mpl::at_c<ps, N>::type                   tp;

	public:
		typedef typename boost::remove_cv<typename boost::remove_reference<tp>::type>::type type;
	};

	template<class F, size_t N>
	static const typename at_<F, N>::type& get_(arg_type const& arg)
	{
		typedef typename at_<F, N>::type type;

		return boost::get<type>(arg);
	}

	template<class F, class WithPropSet, size_t N>
	struct adaptor_;

	template<class F>
	struct adaptor_<F, boost::mpl::true_, 1> {
		F f;

		adaptor_(F f_) : f(f_)
		{ }

		void operator()(args_type const& args, pset_type const& pset) const
		{
			f(get_<F, 0>(args[0]), pset);
		}
	};

	template<class F>
	struct adaptor_<F, boost::mpl::false_, 1> {
		F f;

		adaptor_(F f_) : f(f_)
		{ }

		void operator()(args_type const& args, pset_type const& pset) const
		{
			f(get_<F, 0>(args[0]));
		}
	};

	template<class F>
	struct adaptor_<F, boost::mpl::true_, 2> {
		F f;

		adaptor_(F f_) : f(f_)
		{ }

		void operator()(args_type const& args, pset_type const& pset) const
		{
			f(get_<F, 0>(args[0]), get_<F, 1>(args[1]), pset);
		}
	};

	template<class F>
	struct adaptor_<F, boost::mpl::false_, 2> {
		F f;

		adaptor_(F f_) : f(f_)
		{ }

		void operator()(args_type const& args, pset_type const& pset) const
		{
			f(get_<F, 0>(args[0]), get_<F, 1>(args[1]));
		}
	};

	template<class F>
	struct adaptor_<F, boost::mpl::true_, 3> {
		F f;

		adaptor_(F f_) : f(f_)
		{ }

		void operator()(args_type const& args, pset_type const& pset) const
		{
			f(get_<F, 0>(args[0]), get_<F, 1>(args[1]), get_<F, 2>(args[2]), pset);
		}
	};

	template<class F>
	struct adaptor_<F, boost::mpl::false_, 3> {
		F f;

		adaptor_(F f_) : f(f_)
		{ }

		void operator()(args_type const& args, pset_type const& pset) const
		{
			f(get_<F, 0>(args[0]), get_<F, 1>(args[1]), get_<F, 2>(args[2]));
		}
	};

	template<class F>
	struct adaptor_<F, boost::mpl::true_, 4> {
		F f;

		adaptor_(F f_) : f(f_)
		{ }

		void operator()(args_type const& args, pset_type const& pset) const
		{
			f(get_<F, 0>(args[0]), get_<F, 1>(args[1]), get_<F, 2>(args[2]),
			  get_<F, 3>(args[3]), pset);
		}
	};

	template<class F>
	struct adaptor_<F, boost::mpl::false_, 4> {
		F f;

		adaptor_(F f_) : f(f_)
		{ }

		void operator()(args_type const& args, pset_type const& pset) const
		{
			f(get_<F, 0>(args[0]), get_<F, 1>(args[1]), get_<F, 2>(args[2]),
			  get_<F, 3>(args[3]));
		}
	};

	template<class F>
	struct adaptor_<F, boost::mpl::true_, 5> {
		F f;

		adaptor_(F f_) : f(f_)
		{ }

		void operator()(args_type const& args, pset_type const& pset) const
		{
			f(get_<F, 0>(args[0]), get_<F, 1>(args[1]), get_<F, 2>(args[2]),
			  get_<F, 3>(args[3]), get_<F, 4>(args[4]), pset);
		}
	};

	template<class F>
	struct adaptor_<F, boost::mpl::false_, 5> {
		F f;

		adaptor_(F f_) : f(f_)
		{ }

		void operator()(args_type const& args, pset_type const& pset) const
		{
			f(get_<F, 0>(args[0]), get_<F, 1>(args[1]), get_<F, 2>(args[2]),
			  get_<F, 3>(args[3]), get_<F, 4>(args[4]));
		}
	};

	template<class F, class WithPropSet>
	class traits_;

	template<class F>
	class traits_<F, boost::mpl::true_> {
		typedef typename boost::function_types::parameter_types<F>::type              tmp;
		typedef typename boost::mpl::prior<typename boost::mpl::end<tmp>::type>::type last;

		ODTONE_STATIC_ASSERT(
			(boost::is_same<void,
			                typename boost::function_types::result_type<F>::type>::value),
			"Result type must be void"
		);

		ODTONE_STATIC_ASSERT(
			(boost::is_same<pset_type const&,
			                typename boost::mpl::deref<last>::type>::value),
			"Last argument must a const reference to a pset_type"
		);

	public:
		typedef typename boost::mpl::erase<tmp, last>::type arg_types;

		static const size_t arg_count = boost::mpl::size<arg_types>::value;
	};

	template<class F>
	class traits_<F, boost::mpl::false_> {
		ODTONE_STATIC_ASSERT(
			(boost::is_same<void,
			                typename boost::function_types::result_type<F>::type>::value),
			"Result type must be void"
		);

	public:
		typedef typename boost::function_types::parameter_types<F>::type arg_types;

		static const size_t arg_count = boost::mpl::size<arg_types>::value;
	};

	struct push_arg_id_ {
		push_arg_id_(std::vector<uint>& ais)
			: args_id(ais)
		{ }

		template<class T>
		void operator()(T)
		{
			const uint id = type_id_<T>::value;

			args_id.push_back(id);
		}

		std::vector<uint>& args_id;
	};

public:
	function()
		: _pclass(0), _pcsize(0)
	{}

	template<class F>
	function(F f)
		: _pclass(0), _pcsize(0)
	{
		typedef typename traits_<F, boost::mpl::false_>::arg_types types;

		boost::mpl::for_each<
			types,
			boost::remove_cv< boost::remove_reference<boost::mpl::_1> >
		>(push_arg_id_(_args));

		_func = adaptor_<F, boost::mpl::false_, traits_<F, boost::mpl::false_>::arg_count>(f);
	}

	template<class F, size_t N>
	function(F f, property_class const (&pc)[N])
		: _pclass(pc), _pcsize(N)
	{
		typedef typename traits_<F, boost::mpl::true_>::arg_types types;

		boost::mpl::for_each<
			types,
			boost::remove_cv< boost::remove_reference<boost::mpl::_1> >
		>(push_arg_id_(_args));

		_func = adaptor_<F, boost::mpl::true_, traits_<F, boost::mpl::true_>::arg_count>(f);
	}

	void operator()(args_type const& args, pset_type const& pset) const
	{
		_func(args, pset);
	}

	size_t max_args() const
	{
		return _args.size();
	}

	uint arg_id(uint n) const
	{
		return _args[n];
	}

	property_class const* get_prop_class(std::string const& name) const
	{
		return std::find_if(_pclass, _pclass + _pcsize,
		                    !ph::bind(&std::strcmp, name.c_str(),
		                              ph::bind(&property_class::name, ph::arg_names::arg1)));
	}

private:
	std::vector<uint>     _args;
	property_class const* _pclass;
	size_t                _pcsize;

	boost::function<void(args_type const& args, pset_type const& pset)> _func;
};

typedef std::map<std::string, function> functions;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class Iterator>
struct skipper_grammar : qi::grammar<Iterator> {
	skipper_grammar()
		: skipper_grammar::base_type(sk, "skipper")
	{
		sk =
			  qi::blank
			| ('#'  >> *(qi::standard::char_ - qi::eol) > &qi::eol)
			| ('\\' >> qi::eol)
		;
	}

	qi::rule<Iterator> sk;
};

template<class Iterator>
struct string_grammar : qi::grammar<Iterator, std::string()> {
	string_grammar()
		: string_grammar::base_type(str, "string")
	{
		esc.add("\\\"", '\"')
		       ("\\\\", '\\')
		       ("\\t",  '\t')
		       ("\\n",  '\n')
		       ("\\r",  '\r')
		;

		str =
			   '"'
			>> qi::as_string[*((esc | qi::standard::char_) - '"')][qi::_val = qi::_1]
			>  '"'
		;
	}

	qi::symbols<const char, const char> esc;
	qi::rule<Iterator, std::string()>   str;
};

template<class Iterator>
struct ip4_grammar : qi::grammar<Iterator, net::ip::address_v4()> {

	static net::ip::address_v4 conv(std::string const& str)
	{
		return net::ip::address_v4::from_string(str);
	}

	ip4_grammar()
		: ip4_grammar::base_type(ip4, "ip4")
	{
		ip4 = qi::as_string
		[
			qi::raw
			[
				qi::repeat(3)[u8 >> '.'] >> u8
			]
		]
		[qi::_val = ph::bind(&ip4_grammar::conv, qi::_1)];
	}

	qi::rule<Iterator, net::ip::address_v4()> ip4;
	qi::uint_parser<uint8, 10, 1, 3>          u8;
};

template<class Iterator>
struct ip6_grammar : qi::grammar<Iterator, qi::locals<uint>, net::ip::address_v6()> {
	typedef qi::locals<uint> locals;

	static net::ip::address_v6 conv(std::string const& str)
	{
		return net::ip::address_v6::from_string(str);
	}

	ip6_grammar()
		: ip6_grammar::base_type(ip6, "ip6")
	{
		ip6 = qi::as_string
		[
		  qi::raw
		  [
		      (           qi::repeat(1, 6)[h16 >> ':'] >> ((h16 >>  (':' >> (h16 | ':'))) | ':' | ip4)  )
		    | ( "::" >> -(qi::repeat(0, 5)[h16 >> ':'] >> ((h16 >> -(':' >> (h16      )))       | ip4)) )
		    | (
		           qi::eps[qi::_a = 0]
		        >> +(h16[qi::_pass = ++qi::_a < 7u] >> ':')
		        >> +(':' >> h16[qi::_pass = ++qi::_a < 8u])
		      )
		  ]
		]
		[qi::_val = ph::bind(&ip6_grammar::conv, qi::_1)];
	}

	qi::rule<Iterator, locals, net::ip::address_v6()> ip6;
	qi::uint_parser<uint16, 16, 1, 4>                 h16;
	ip4_grammar<Iterator>                             ip4;
};

template<class Iterator>
struct parser_grammar : qi::grammar<Iterator,
                                    skipper_grammar<Iterator>
                                    > {
	typedef skipper_grammar<Iterator> skipper;

	struct act_on_name_ {
		template<class, class, class, class>
		struct result { typedef bool type; };

		bool operator()(function const*& f, functions const& cm, std::string const& name, error_reason& er) const
		{
			functions::const_iterator it = cm.find(name);

			if (it == cm.end()) {
				er = invalid_command;
				return false;
			}

			f = &it->second;
			return true;
		}
	};

	struct act_on_arg_ {
		template<class, class, class, class>
		struct result { typedef bool type; };

		bool operator()(function const& f, args_type& args, arg_type& arg, error_reason& er) const
		{
			const uint n = args.size();

			if (n >= f.max_args()) {
				er = to_many_args;
			    return false;
			}

			if (static_cast<uint>(arg.which()) != f.arg_id(n)) {
				er = invalid_arg_type;
				return false;
			}

			args.push_back(arg);
			return true;
		}
	};

	struct act_on_prop_ {
		template<class, class, class, class>
		struct result { typedef bool type; };

		bool operator()(property_class const*& pc, function const& f, std::string const& name, error_reason& er) const
		{
			pc = f.get_prop_class(name);

			if (!pc) {
				er = invalid_prop;
				return false;
			}
			return true;
		}

		bool operator()(property_class const* pc, arg_type const& arg, uint& count, error_reason& er) const
		{
			if (++count > pc->count) {
				er = to_many_prop_args;
				return false;
			}
			if (pc->type_id != uint(arg.which())) {
				er = invalid_prop_arg_type;
				return false;
			}
			return true;
		}
	};

	struct run_cmd_ {
		template<class, class, class>
		struct result { typedef void type; };

		void operator()(function const& f, args_type const& args, pset_type const& pset) const
		{
			f(args, pset);
		}
	};

	struct error_handler_ {
		template<class, class, class, class, class>
		struct result { typedef void type; };

		void operator()(Iterator begin, Iterator end, Iterator pos, qi::info const& what, error_reason er) const
		{
			Iterator eol = std::find(begin, end, '\n');

			std::cout << "error: "
			          << error_reason_string[er]
			          << ", expecting \'"
			          << what
			          << "\':\n"
			          << std::string(begin, eol)
			          << std::endl
			          << std::string(std::distance(begin, pos), '~')
			          << '^'
			          << std::string(std::distance(pos, eol), '~')
			          << std::endl;
		}
	};

	parser_grammar(functions const& cm)
		: parser_grammar::base_type(start), cmds(cm), ereason(invalid_syntax)
	{
		start = qi::eol | cmd;

		cmd = (
		      name           [qi::_pass = act_on_name(qi::_a, ph::cref(cmds), qi::_1, ph::ref(ereason))]
			> *arg           [qi::_pass = act_on_arg(*qi::_a, qi::_b, qi::_1, ph::ref(ereason))]
			> -pset(*qi::_a) [qi::_c = qi::_1]
			> qi::eol
		)
		[run_cmd(*qi::_a, qi::_b, qi::_c)];

		name %= qi::lexeme[qi::alpha >> *(qi::alnum | '-')];

		arg %=
		    ip4
		  | ip6
		  | qi::uint_
		  | qi::int_
		  | qi::double_
		  | str
		;

		pset =
			  qi::lit('{')
			> qi::eol
			> +prop(qi::_r1)
			> qi::lit('}')
		;

		prop =
			   name[qi::_val = qi::_1, qi::_a = 0,
			        qi::_pass = act_on_prop(qi::_a, qi::_r1, qi::_1, ph::ref(ereason))]
			> (*arg[qi::_val = qi::_1,
			        qi::_pass = act_on_prop(qi::_a, qi::_1, qi::_b, ph::ref(ereason))])
			> qi::eol
		;

		cmd.name("command");
		name.name("identifier");
		arg.name("argument");
		pset.name("property-set");
		prop.name("property");

		qi::on_error<qi::fail>(start, error_handler(qi::_1, qi::_2, qi::_3, qi::_4, ph::ref(ereason)));
	}

	typedef qi::locals<function const*, args_type, pset_type> cmd_locals;
	typedef qi::locals<property_class const*, uint>           prop_locals;

	qi::rule<Iterator,                                          skipper> start;
	qi::rule<Iterator, cmd_locals,                              skipper> cmd;
	qi::rule<Iterator, std::string(),                           skipper> name;
	qi::rule<Iterator, arg_type(),                              skipper> arg;
	qi::rule<Iterator, pset_type(function const&),              skipper> pset;
	qi::rule<Iterator, prop_type(function const&), prop_locals, skipper> prop;

	string_grammar<Iterator> str;
	ip4_grammar<Iterator>    ip4;
	ip6_grammar<Iterator>    ip6;

	functions const& cmds;
	error_reason     ereason;

	ph::function<act_on_name_>   act_on_name;
	ph::function<act_on_arg_>    act_on_arg;
	ph::function<act_on_prop_>   act_on_prop;
	ph::function<run_cmd_>       run_cmd;
	ph::function<error_handler_> error_handler;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
bool exec(std::string::const_iterator& begin, std::string::const_iterator end, functions const& cm);

/////////////////////////////////////////////////////////////////////////////////////////////////////////
} /* namespace conf */ } /* namespace odtone */

// EOF //////////////////////////////////////////////////////////////////////////////////////////////////
#endif /* ODTONE_PMIP_CONF__HPP_ */
