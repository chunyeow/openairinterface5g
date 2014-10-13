//=============================================================================
// Brief   : Archive details
// Authors : Bruno Santos <bsantos@av.it.pt>
//------------------------------------------------------------------------------
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
//==============================================================================

#ifndef ODTONE_MIH_DETAIL_ARCHIVE__HPP_
#define ODTONE_MIH_DETAIL_ARCHIVE__HPP_

///////////////////////////////////////////////////////////////////////////////
#include <odtone/cast.hpp>
#include <boost/array.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/get.hpp>
#include <vector>
#include <list>

///////////////////////////////////////////////////////////////////////////////
namespace odtone { namespace mih { namespace detail {

///////////////////////////////////////////////////////////////////////////////
template<class T>
struct serialize {
	template<class ArchiveT>
	void operator()(ArchiveT& ar, T& val) const
	{
		val.serialize(ar);
	}
};

template<class T>
struct serialize<std::vector<T> > {
	void operator()(iarchive& ar, std::vector<T>& val) const
	{
		uint pos = ar.position();
		uint len = ar.list_length();

		val.resize(len);
		try {
			typename std::vector<T>::iterator it = val.begin();

			while (len--) {
				ar & *it;
				++it;
			}

		} catch (...) {
			val.clear();
			ar.position(pos);
			throw;
		}
	}

	void operator()(oarchive& ar, std::vector<T>& val) const
	{
		typename std::vector<T>::iterator it = val.begin();
		uint len = truncate_cast<uint>(val.size());
		uint pos = ar.position();

		ar.list_length(len);
		try {
			while (len--) {
				ar & *it;
				++it;
			}

		} catch (...) {
			ar.position(pos);
			throw;
		}
	}
};

template<class T>
struct serialize<std::list<T> > {
	void operator()(iarchive& ar, std::list<T>& val) const
	{
		uint pos = ar.position();
		uint len = ar.list_length();

		val.resize(len);
		try {
			typename std::list<T>::iterator it = val.begin();

			while (len--) {
				ar & *it;
				++it;
			}

		} catch (...) {
			val.clear();
			ar.position(pos);
			throw;
		}
	}

	void operator()(oarchive& ar, std::list<T>& val) const
	{
		typename std::list<T>::iterator it = val.begin();
		uint len = truncate_cast<uint>(val.size());
		uint pos = ar.position();

		ar.list_length(len);
		try {
			while (len--) {
				ar & *it;
				++it;
			}

		} catch (...) {
			ar.position(pos);
			throw;
		}
	}
};

template<class T, size_t N>
struct serialize<boost::array<T, N> > {
	typedef typename boost::array<T, N> value_type;

	template<class Archive>
	void operator()(Archive& ar, value_type& val) const
	{
		for (size_t i = 0; i < N; ++i)
			ar & val.elems[i];
	}
};

template<class T1>
struct serialize<boost::variant<T1> > {
	void operator()(iarchive& ar, boost::variant<T1>& val) const
	{
		octet selector;

		ar & selector;
		switch (selector) {
		case 0:
			val = T1();
			ar & boost::get<T1>(val);
			break;

	//	default:
	//		ODTONE_NEVER_HERE;
		}
	}

	void operator()(oarchive& ar, boost::variant<T1>& val) const
	{
		octet selector = val.which();

		ar & selector;
		switch (selector) {
		case 0:
			ar & boost::get<T1>(val);
			break;

	//	default:
	//		ODTONE_NEVER_HERE;
		}
	}
};

template<class T1, class T2>
struct serialize<boost::variant<T1, T2> > {
	void operator()(iarchive& ar, boost::variant<T1, T2>& val) const
	{
		octet selector;

		ar & selector;
		switch (selector) {
		case 0:
			val = T1();
			ar & boost::get<T1>(val);
			break;

		case 1:
			val = T2();
			ar & boost::get<T2>(val);
			break;

	//	default:
	//		ODTONE_NEVER_HERE;
		}
	}

	void operator()(oarchive& ar, boost::variant<T1, T2>& val) const
	{
		octet selector = val.which();

		ar & selector;
		switch (selector) {
		case 0:
			ar & boost::get<T1>(val);
			break;

		case 1:
			ar & boost::get<T2>(val);
			break;

	//	default:
	//		ODTONE_NEVER_HERE;
		}
	}
};

template<class T1, class T2, class T3>
struct serialize<boost::variant<T1, T2, T3> > {
	void operator()(iarchive& ar, boost::variant<T1, T2, T3>& val) const
	{
		octet selector;

		ar & selector;
		switch (selector) {
		case 0:
			val = T1();
			ar & boost::get<T1>(val);
			break;

		case 1:
			val = T2();
			ar & boost::get<T2>(val);
			break;

		case 2:
			val = T3();
			ar & boost::get<T3>(val);
			break;

	//	default:
	//		ODTONE_NEVER_HERE;
		}
	}

	void operator()(oarchive& ar, boost::variant<T1, T2, T3>& val) const
	{
		octet selector = val.which();

		ar & selector;
		switch (selector) {
		case 0:
			ar & boost::get<T1>(val);
			break;

		case 1:
			ar & boost::get<T2>(val);
			break;

		case 2:
			ar & boost::get<T3>(val);
			break;

	//	default:
	//		ODTONE_NEVER_HERE;
		}
	}
};

template<class T1, class T2, class T3, class T4>
struct serialize<boost::variant<T1, T2, T3, T4> > {
	void operator()(iarchive& ar, boost::variant<T1, T2, T3, T4>& val) const
	{
		octet selector;

		ar & selector;
		switch (selector) {
		case 0:
			val = T1();
			ar & boost::get<T1>(val);
			break;

		case 1:
			val = T2();
			ar & boost::get<T2>(val);
			break;

		case 2:
			val = T3();
			ar & boost::get<T3>(val);
			break;

		case 3:
			val = T4();
			ar & boost::get<T4>(val);
			break;

	//	default:
	//		ODTONE_NEVER_HERE;
		}
	}

	void operator()(oarchive& ar, boost::variant<T1, T2, T3, T4>& val) const
	{
		octet selector = val.which();

		ar & selector;
		switch (selector) {
		case 0:
			ar & boost::get<T1>(val);
			break;

		case 1:
			ar & boost::get<T2>(val);
			break;

		case 2:
			ar & boost::get<T3>(val);
			break;

		case 3:
			ar & boost::get<T4>(val);
			break;

	//	default:
	//		ODTONE_NEVER_HERE;
		}
	}
};

template<class T1, class T2, class T3, class T4, class T5>
struct serialize<boost::variant<T1, T2, T3, T4, T5> > {
	void operator()(iarchive& ar, boost::variant<T1, T2, T3, T4, T5>& val) const
	{
		octet selector;

		ar & selector;
		switch (selector) {
		case 0:
			val = T1();
			ar & boost::get<T1>(val);
			break;

		case 1:
			val = T2();
			ar & boost::get<T2>(val);
			break;

		case 2:
			val = T3();
			ar & boost::get<T3>(val);
			break;

		case 3:
			val = T4();
			ar & boost::get<T4>(val);
			break;

		case 4:
			val = T5();
			ar & boost::get<T5>(val);
			break;

	//	default:
	//		ODTONE_NEVER_HERE;
		}
	}

	void operator()(oarchive& ar, boost::variant<T1, T2, T3, T4, T5>& val) const
	{
		octet selector = val.which();

		ar & selector;
		switch (selector) {
		case 0:
			ar & boost::get<T1>(val);
			break;

		case 1:
			ar & boost::get<T2>(val);
			break;

		case 2:
			ar & boost::get<T3>(val);
			break;

		case 3:
			ar & boost::get<T4>(val);
			break;

		case 4:
			ar & boost::get<T5>(val);
			break;

	//	default:
	//		ODTONE_NEVER_HERE;
		}
	}
};

template<class T1, class T2, class T3, class T4, class T5, class T6>
struct serialize<boost::variant<T1, T2, T3, T4, T5, T6> > {
	void operator()(iarchive& ar, boost::variant<T1, T2, T3, T4, T5, T6>& val) const
	{
		octet selector;

		ar & selector;
		switch (selector) {
		case 0:
			val = T1();
			ar & boost::get<T1>(val);
			break;

		case 1:
			val = T2();
			ar & boost::get<T2>(val);
			break;

		case 2:
			val = T3();
			ar & boost::get<T3>(val);
			break;

		case 3:
			val = T4();
			ar & boost::get<T4>(val);
			break;

		case 4:
			val = T5();
			ar & boost::get<T5>(val);
			break;

		case 5:
			val = T6();
			ar & boost::get<T6>(val);
			break;

	//	default:
	//		ODTONE_NEVER_HERE;
		}
	}

	void operator()(oarchive& ar, boost::variant<T1, T2, T3, T4, T5, T6>& val) const
	{
		octet selector = val.which();

		ar & selector;
		switch (selector) {
		case 0:
			ar & boost::get<T1>(val);
			break;

		case 1:
			ar & boost::get<T2>(val);
			break;

		case 2:
			ar & boost::get<T3>(val);
			break;

		case 3:
			ar & boost::get<T4>(val);
			break;

		case 4:
			ar & boost::get<T5>(val);
			break;

		case 5:
			ar & boost::get<T6>(val);
			break;

	//	default:
	//		ODTONE_NEVER_HERE;
		}
	}
};

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
struct serialize<boost::variant<T1, T2, T3, T4, T5, T6, T7> > {
	void operator()(iarchive& ar, boost::variant<T1, T2, T3, T4, T5, T6, T7>& val) const
	{
		octet selector;

		ar & selector;
		switch (selector) {
		case 0:
			val = T1();
			ar & boost::get<T1>(val);
			break;

		case 1:
			val = T2();
			ar & boost::get<T2>(val);
			break;

		case 2:
			val = T3();
			ar & boost::get<T3>(val);
			break;

		case 3:
			val = T4();
			ar & boost::get<T4>(val);
			break;

		case 4:
			val = T5();
			ar & boost::get<T5>(val);
			break;

		case 5:
			val = T6();
			ar & boost::get<T6>(val);
			break;

		case 6:
			val = T7();
			ar & boost::get<T7>(val);
			break;

	//	default:
	//		ODTONE_NEVER_HERE;
		}
	}

	void operator()(oarchive& ar, boost::variant<T1, T2, T3, T4, T5, T6, T7>& val) const
	{
		octet selector = val.which();

		ar & selector;
		switch (selector) {
		case 0:
			ar & boost::get<T1>(val);
			break;

		case 1:
			ar & boost::get<T2>(val);
			break;

		case 2:
			ar & boost::get<T3>(val);
			break;

		case 3:
			ar & boost::get<T4>(val);
			break;

		case 4:
			ar & boost::get<T5>(val);
			break;

		case 5:
			ar & boost::get<T6>(val);
			break;

		case 6:
			ar & boost::get<T7>(val);
			break;

	//	default:
	//		ODTONE_NEVER_HERE;
		}
	}
};

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
struct serialize<boost::variant<T1, T2, T3, T4, T5, T6, T7, T8> > {
	void operator()(iarchive& ar, boost::variant<T1, T2, T3, T4, T5, T6, T7, T8>& val) const
	{
		octet selector;

		ar & selector;
		switch (selector) {
		case 0:
			val = T1();
			ar & boost::get<T1>(val);
			break;

		case 1:
			val = T2();
			ar & boost::get<T2>(val);
			break;

		case 2:
			val = T3();
			ar & boost::get<T3>(val);
			break;

		case 3:
			val = T4();
			ar & boost::get<T4>(val);
			break;

		case 4:
			val = T5();
			ar & boost::get<T5>(val);
			break;

		case 5:
			val = T6();
			ar & boost::get<T6>(val);
			break;

		case 6:
			val = T7();
			ar & boost::get<T7>(val);
			break;

		case 7:
			val = T8();
			ar & boost::get<T8>(val);
			break;


	//	default:
	//		ODTONE_NEVER_HERE;
		}
	}

	void operator()(oarchive& ar, boost::variant<T1, T2, T3, T4, T5, T6, T7, T8>& val) const
	{
		octet selector = val.which();

		ar & selector;
		switch (selector) {
		case 0:
			ar & boost::get<T1>(val);
			break;

		case 1:
			ar & boost::get<T2>(val);
			break;

		case 2:
			ar & boost::get<T3>(val);
			break;

		case 3:
			ar & boost::get<T4>(val);
			break;

		case 4:
			ar & boost::get<T5>(val);
			break;

		case 5:
			ar & boost::get<T6>(val);
			break;

		case 6:
			ar & boost::get<T7>(val);
			break;

		case 7:
			ar & boost::get<T8>(val);
			break;

	//	default:
	//		ODTONE_NEVER_HERE;
		}
	}
};

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct serialize<boost::variant<T1, T2, T3, T4, T5, T6, T7, T8, T9> > {
	void operator()(iarchive& ar, boost::variant<T1, T2, T3, T4, T5, T6, T7, T8, T9>& val) const
	{
		octet selector;

		ar & selector;
		switch (selector) {
		case 0:
			val = T1();
			ar & boost::get<T1>(val);
			break;

		case 1:
			val = T2();
			ar & boost::get<T2>(val);
			break;

		case 2:
			val = T3();
			ar & boost::get<T3>(val);
			break;

		case 3:
			val = T4();
			ar & boost::get<T4>(val);
			break;

		case 4:
			val = T5();
			ar & boost::get<T5>(val);
			break;

		case 5:
			val = T6();
			ar & boost::get<T6>(val);
			break;

		case 6:
			val = T7();
			ar & boost::get<T7>(val);
			break;

		case 7:
			val = T8();
			ar & boost::get<T8>(val);
			break;

		case 8:
			val = T9();
			ar & boost::get<T9>(val);
			break;


	//	default:
	//		ODTONE_NEVER_HERE;
		}
	}

	void operator()(oarchive& ar, boost::variant<T1, T2, T3, T4, T5, T6, T7, T8, T9>& val) const
	{
		octet selector = val.which();

		ar & selector;
		switch (selector) {
		case 0:
			ar & boost::get<T1>(val);
			break;

		case 1:
			ar & boost::get<T2>(val);
			break;

		case 2:
			ar & boost::get<T3>(val);
			break;

		case 3:
			ar & boost::get<T4>(val);
			break;

		case 4:
			ar & boost::get<T5>(val);
			break;

		case 5:
			ar & boost::get<T6>(val);
			break;

		case 6:
			ar & boost::get<T7>(val);
			break;

		case 7:
			ar & boost::get<T8>(val);
			break;

		case 8:
			ar & boost::get<T9>(val);
			break;


	//	default:
	//		ODTONE_NEVER_HERE;
		}
	}
};

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
struct serialize<boost::variant<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10> > {
	void operator()(iarchive& ar, boost::variant<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>& val) const
	{
		octet selector;

		ar & selector;
		switch (selector) {
		case 0:
			val = T1();
			ar & boost::get<T1>(val);
			break;

		case 1:
			val = T2();
			ar & boost::get<T2>(val);
			break;

		case 2:
			val = T3();
			ar & boost::get<T3>(val);
			break;

		case 3:
			val = T4();
			ar & boost::get<T4>(val);
			break;

		case 4:
			val = T5();
			ar & boost::get<T5>(val);
			break;

		case 5:
			val = T6();
			ar & boost::get<T6>(val);
			break;

		case 6:
			val = T7();
			ar & boost::get<T7>(val);
			break;

		case 7:
			val = T8();
			ar & boost::get<T8>(val);
			break;

		case 8:
			val = T9();
			ar & boost::get<T9>(val);
			break;

		case 9:
			val = T10();
			ar & boost::get<T10>(val);
			break;


	//	default:
	//		ODTONE_NEVER_HERE;
		}
	}

	void operator()(oarchive& ar, boost::variant<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>& val) const
	{
		octet selector = val.which();

		ar & selector;
		switch (selector) {
		case 0:
			ar & boost::get<T1>(val);
			break;

		case 1:
			ar & boost::get<T2>(val);
			break;

		case 2:
			ar & boost::get<T3>(val);
			break;

		case 3:
			ar & boost::get<T4>(val);
			break;

		case 4:
			ar & boost::get<T5>(val);
			break;

		case 5:
			ar & boost::get<T6>(val);
			break;

		case 6:
			ar & boost::get<T7>(val);
			break;

		case 7:
			ar & boost::get<T8>(val);
			break;

		case 8:
			ar & boost::get<T9>(val);
			break;

		case 9:
			ar & boost::get<T10>(val);
			break;


	//	default:
	//		ODTONE_NEVER_HERE;
		}
	}
};

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11>
struct serialize<boost::variant<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11> > {
	void operator()(iarchive& ar, boost::variant<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>& val) const
	{
		octet selector;

		ar & selector;
		switch (selector) {
		case 0:
			val = T1();
			ar & boost::get<T1>(val);
			break;

		case 1:
			val = T2();
			ar & boost::get<T2>(val);
			break;

		case 2:
			val = T3();
			ar & boost::get<T3>(val);
			break;

		case 3:
			val = T4();
			ar & boost::get<T4>(val);
			break;

		case 4:
			val = T5();
			ar & boost::get<T5>(val);
			break;

		case 5:
			val = T6();
			ar & boost::get<T6>(val);
			break;

		case 6:
			val = T7();
			ar & boost::get<T7>(val);
			break;

		case 7:
			val = T8();
			ar & boost::get<T8>(val);
			break;

		case 8:
			val = T9();
			ar & boost::get<T9>(val);
			break;

		case 9:
			val = T10();
			ar & boost::get<T10>(val);
			break;

		case 10:
			val = T11();
			ar & boost::get<T11>(val);
			break;


	//	default:
	//		ODTONE_NEVER_HERE;
		}
	}

	void operator()(oarchive& ar, boost::variant<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>& val) const
	{
		octet selector = val.which();

		ar & selector;
		switch (selector) {
		case 0:
			ar & boost::get<T1>(val);
			break;

		case 1:
			ar & boost::get<T2>(val);
			break;

		case 2:
			ar & boost::get<T3>(val);
			break;

		case 3:
			ar & boost::get<T4>(val);
			break;

		case 4:
			ar & boost::get<T5>(val);
			break;

		case 5:
			ar & boost::get<T6>(val);
			break;

		case 6:
			ar & boost::get<T7>(val);
			break;

		case 7:
			ar & boost::get<T8>(val);
			break;

		case 8:
			ar & boost::get<T9>(val);
			break;

		case 9:
			ar & boost::get<T10>(val);
			break;

		case 10:
			ar & boost::get<T11>(val);
			break;

	//	default:
	//		ODTONE_NEVER_HERE;
		}
	}
};

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12>
struct serialize<boost::variant<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12> > {
	void operator()(iarchive& ar, boost::variant<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12>& val) const
	{
		octet selector;

		ar & selector;
		switch (selector) {
		case 0:
			val = T1();
			ar & boost::get<T1>(val);
			break;

		case 1:
			val = T2();
			ar & boost::get<T2>(val);
			break;

		case 2:
			val = T3();
			ar & boost::get<T3>(val);
			break;

		case 3:
			val = T4();
			ar & boost::get<T4>(val);
			break;

		case 4:
			val = T5();
			ar & boost::get<T5>(val);
			break;

		case 5:
			val = T6();
			ar & boost::get<T6>(val);
			break;

		case 6:
			val = T7();
			ar & boost::get<T7>(val);
			break;

		case 7:
			val = T8();
			ar & boost::get<T8>(val);
			break;

		case 8:
			val = T9();
			ar & boost::get<T9>(val);
			break;

		case 9:
			val = T10();
			ar & boost::get<T10>(val);
			break;

		case 10:
			val = T11();
			ar & boost::get<T11>(val);
			break;

		case 11:
			val = T12();
			ar & boost::get<T12>(val);
			break;

	//	default:
	//		ODTONE_NEVER_HERE;
		}
	}

	void operator()(oarchive& ar, boost::variant<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12>& val) const
	{
		octet selector = val.which();

		ar & selector;
		switch (selector) {
		case 0:
			ar & boost::get<T1>(val);
			break;

		case 1:
			ar & boost::get<T2>(val);
			break;

		case 2:
			ar & boost::get<T3>(val);
			break;

		case 3:
			ar & boost::get<T4>(val);
			break;

		case 4:
			ar & boost::get<T5>(val);
			break;

		case 5:
			ar & boost::get<T6>(val);
			break;

		case 6:
			ar & boost::get<T7>(val);
			break;

		case 7:
			ar & boost::get<T8>(val);
			break;

		case 8:
			ar & boost::get<T9>(val);
			break;

		case 9:
			ar & boost::get<T10>(val);
			break;

		case 10:
			ar & boost::get<T11>(val);
			break;

		case 11:
			ar & boost::get<T12>(val);
			break;

	//	default:
	//		ODTONE_NEVER_HERE;
		}
	}
};


template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13>
struct serialize<boost::variant<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13> > {
	void operator()(iarchive& ar, boost::variant<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13>& val) const
	{
		octet selector;

		ar & selector;
		switch (selector) {
		case 0:
			val = T1();
			ar & boost::get<T1>(val);
			break;

		case 1:
			val = T2();
			ar & boost::get<T2>(val);
			break;

		case 2:
			val = T3();
			ar & boost::get<T3>(val);
			break;

		case 3:
			val = T4();
			ar & boost::get<T4>(val);
			break;

		case 4:
			val = T5();
			ar & boost::get<T5>(val);
			break;

		case 5:
			val = T6();
			ar & boost::get<T6>(val);
			break;

		case 6:
			val = T7();
			ar & boost::get<T7>(val);
			break;

		case 7:
			val = T8();
			ar & boost::get<T8>(val);
			break;

		case 8:
			val = T9();
			ar & boost::get<T9>(val);
			break;

		case 9:
			val = T10();
			ar & boost::get<T10>(val);
			break;

		case 10:
			val = T11();
			ar & boost::get<T11>(val);
			break;

		case 11:
			val = T12();
			ar & boost::get<T12>(val);
			break;

		case 12:
			val = T13();
			ar & boost::get<T13>(val);
			break;

	//	default:
	//		ODTONE_NEVER_HERE;
		}
	}

	void operator()(oarchive& ar, boost::variant<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13>& val) const
	{
		octet selector = val.which();

		ar & selector;
		switch (selector) {
		case 0:
			ar & boost::get<T1>(val);
			break;

		case 1:
			ar & boost::get<T2>(val);
			break;

		case 2:
			ar & boost::get<T3>(val);
			break;

		case 3:
			ar & boost::get<T4>(val);
			break;

		case 4:
			ar & boost::get<T5>(val);
			break;

		case 5:
			ar & boost::get<T6>(val);
			break;

		case 6:
			ar & boost::get<T7>(val);
			break;

		case 7:
			ar & boost::get<T8>(val);
			break;

		case 8:
			ar & boost::get<T9>(val);
			break;

		case 9:
			ar & boost::get<T10>(val);
			break;

		case 10:
			ar & boost::get<T11>(val);
			break;

		case 11:
			ar & boost::get<T12>(val);
			break;

		case 12:
			ar & boost::get<T13>(val);
			break;

	//	default:
	//		ODTONE_NEVER_HERE;
		}
	}
};

///////////////////////////////////////////////////////////////////////////////
} /* namspace detail */

///////////////////////////////////////////////////////////////////////////////
template<class T>
inline iarchive& operator&(iarchive& ar, T& val)
{
	detail::serialize<T>()(ar, val);
	return ar;
}

template<class T>
inline oarchive& operator&(oarchive& ar, T& val)
{
	detail::serialize<T>()(ar, val);
	return ar;
}

///////////////////////////////////////////////////////////////////////////////
} /* namespace mih */ } /*namespace odtone */

// EOF ////////////////////////////////////////////////////////////////////////
#endif /* ODTONE_MIH_DETAIL_ARCHIVE__HPP_ */
