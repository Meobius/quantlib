
/*
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email quantlib-dev@lists.sf.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

/*! \file relinkablehandle.hpp
    \brief Globally accessible relinkable pointer
*/

#ifndef quantlib_relinkable_handle_h
#define quantlib_relinkable_handle_h

#include <ql/Patterns/observable.hpp>

namespace QuantLib {

    //! Relinkable access to a Handle
    /*! \pre Class "Type" must inherit from Observable */
    template <class Type>
    class Link : public Observable, public Observer {
      public:
        /*! \warning see the documentation of the <tt>linkTo</tt> method
                for issues relatives to <tt>registerAsObserver</tt>.
        */
        explicit Link(const Handle<Type>& h = Handle<Type>(),
             bool registerAsObserver = true);
        /*! \warning <i><b>registerAsObserver</b></i> is left as a backdoor
                in case the programmer cannot guarantee that the object
                pointed to will remain alive for the whole lifetime of the
                handle---namely, it should be set to <tt>false</tt> when the
                passed handle was created with <tt>owns = false</tt> (the
                latter should only happen in a controlled environment, so
                that the programmer is aware of it). Failure to do so can
                very likely result in a program crash.
                If the programmer does want the relinkable handle to register
                as observer of such a handle, it is his responsibility to
                ensure that the relinkable handle gets destroyed before the
                pointed object does.
        */
        void linkTo(const Handle<Type>& h, bool registerAsObserver = true);
        //! Checks if the contained handle points to anything
        bool isNull() const { return h_.isNull(); }
        //! Returns the contained handle
        const Handle<Type>& currentLink() const { return h_; }
        //! Observer interface
        void update() { notifyObservers(); }
      private:
        Handle<Type> h_;
        bool isObserver_;
    };


    //! Globally accessible relinkable pointer
    /*! An instance of this class can be relinked to another Handle: such 
        change will be propagated to all the copies of the instance.
        \pre Class "Type" must inherit from Observable
    */
    template <class Type>
    class RelinkableHandle : public Handle<Link<Type> > {
      public:
        /*! \warning see the documentation of <tt>Link</tt> for issues 
                     relatives to <tt>registerAsObserver</tt>.
        */
        explicit RelinkableHandle(const Handle<Type>& h = Handle<Type>(),
                                  bool registerAsObserver = true);
        /*! \warning see the documentation of <tt>Link</tt> for issues 
                     relatives to <tt>registerAsObserver</tt>.
        */
        void linkTo(const Handle<Type>& h, bool registerAsObserver = true);
        //! dereferencing
        const Handle<Type>& operator->() const;
        //! Checks if the contained handle points to anything
        bool isNull() const;
    };


    // inline definitions

    template <class Type>
    inline Link<Type>::Link(const Handle<Type>& h, bool registerAsObserver) 
    : isObserver_(false) {
        linkTo(h,registerAsObserver);
    }

    template <class Type>
    inline void Link<Type>::linkTo(const Handle<Type>& h,
                                   bool registerAsObserver) {
        if ((h != h_) || (isObserver_ != registerAsObserver)) {
            if (!h_.isNull() && isObserver_) {
                unregisterWith(h_);
            }
            h_ = h;
            isObserver_ = registerAsObserver;
            if (!h_.isNull() && isObserver_) {
                registerWith(h_);
            }
            notifyObservers();
        }
    }



    template <class Type>
    inline RelinkableHandle<Type>::RelinkableHandle(const Handle<Type>& h, 
                                                    bool registerAsObserver) 
    : Handle<Link<Type> >(new Link<Type>(h,registerAsObserver)) {}

    template <class Type>
    inline void RelinkableHandle<Type>::linkTo(const Handle<Type>& h,
                                               bool registerAsObserver) {
        (**this).linkTo(h,registerAsObserver);
    }

    template <class Type>
    inline const Handle<Type>& RelinkableHandle<Type>::operator->() const {
        return (**this).currentLink();
    }

    template <class Type>
    inline bool RelinkableHandle<Type>::isNull() const {
        return (**this).isNull();
    }

}


#endif
