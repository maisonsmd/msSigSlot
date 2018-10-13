// msSigSlot.h

#ifndef _MSSIGSLOT_h
#define _MSSIGSLOT_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

enum SlotType { Function, Method };

template <typename ...ParamsT>
class Slot {
protected:
	SlotType _slotType;
	Slot(SlotType slotType) : _slotType(slotType) {}
public:
	virtual ~Slot() {}

	virtual void operator()(ParamsT... params) const = 0;

	virtual bool operator==(const Slot<ParamsT...>* slot) const = 0;

	virtual Slot<ParamsT...>* clone() const = 0;

	SlotType slotType() const {
		return _slotType;
	}
};

template <typename Signature>
class FunctionSlot;

template <typename ReturnT, typename ...ParamsT>
class FunctionSlot<ReturnT(ParamsT...)> : public Slot<ParamsT...> {
private:
	typedef ReturnT(*FuncPtr)(ParamsT...);
	FuncPtr _funcPtr;
public:
	FunctionSlot(FuncPtr f) : Slot<ParamsT...>(SlotType::Function), _funcPtr(f) {}

	Slot<ParamsT...> *clone() const {
		return new FunctionSlot<ReturnT(ParamsT...)>(_funcPtr);
	}

	void operator() (ParamsT... params) const {
		(_funcPtr)(params...);
	}

	bool operator==(const Slot<ParamsT...>* slot) const {
		if (slot && slot->slotType() == Slot<ParamsT... >::_slotType) {
			const FunctionSlot<ReturnT(ParamsT...)>* functSlot = reinterpret_cast<const FunctionSlot<ReturnT(ParamsT...)>*>(slot);
			return functSlot && functSlot->_funcPtr == this->_funcPtr;
		}
		return false;
	}
};

template <typename ObjectT, typename Signature>
class MethodSlot;

template <typename ObjectT, typename ReturnT, typename ...ParamsT>
class MethodSlot<ObjectT, ReturnT(ParamsT...)> : public Slot<ParamsT...> {
private:
	typedef ReturnT(ObjectT::*FuncPtr)(ParamsT...);

	ObjectT *_obj;

	FuncPtr _funcPrt;
public:
	MethodSlot(ObjectT *obj, FuncPtr f) : Slot<ParamsT...>(SlotType::Method), _obj(obj), _funcPrt(f) { }

	Slot<ParamsT...> * clone() const {
		return new MethodSlot<ObjectT, ReturnT(ParamsT...)>(_obj, _funcPrt);
	}

	void operator() (ParamsT... params) const {
		return (_obj->*_funcPrt)(params...);
	}

	bool operator==(const Slot<ParamsT...>* slot) const {
		if (slot && slot->slotType() == Slot<ParamsT...>::_slotType) {
			const MethodSlot<ObjectT, ReturnT(ParamsT...)>* methSlot = reinterpret_cast<const MethodSlot<ObjectT, ReturnT(ParamsT...)>*>(slot);
			return methSlot && methSlot->_obj == _obj && methSlot->_funcPrt == _funcPrt;
		}
		return false;
	}
};

template <typename Signature, int8_t Slots = 1>
class Signal;

template <int8_t Slots, typename ReturnT, typename ...ParamsT> class Signal <ReturnT(ParamsT...), Slots> {
private:
	Slot<ParamsT...> * _connections[Slots];
	int8_t _slotsCount = 0;

public:
	virtual ~Signal() {
		for (uint8_t i = 0; i < _slotsCount; i++)
			delete _connections[i];
	}

	Signal & attach(const Slot<ParamsT...>& slot) {
		if (_slotsCount < Slots) {
			_connections[_slotsCount++] = slot.clone();
		}
		return *this;
	}

	Signal & attach(ReturnT(*func)(ParamsT...)) {
		return attach(FunctionSlot< ReturnT(ParamsT...)>(func));
	}

	template <typename ObjectT>
	Signal & attach(ObjectT *obj, ReturnT(ObjectT::*method)(ParamsT...)) {
		return attach(MethodSlot<ObjectT, ReturnT(ParamsT...)>(obj, method));
	}

	Signal & detach(const Slot<ParamsT...>& slot) {
		for (int8_t i = _slotsCount - 1; i >= 0; i--) {
			if (slot == _connections[i]) {
				delete _connections[i];

				for (int j = i; j < _slotsCount - 1; j++)
					_connections[j] = _connections[j + 1];

				_connections[_slotsCount] = NULL;

				_slotsCount--;
			}
		}
		return *this;
	}

	Signal & detach(ReturnT(*func)(ParamsT...)) {
		return detach(FunctionSlot< ReturnT(ParamsT...)>(func));
	}

	template <typename ObjectT>
	Signal & detach(ObjectT *obj, ReturnT(ObjectT::*method)(ParamsT...)) {
		return detach(MethodSlot<ObjectT, ReturnT(ParamsT...)>(obj, method));
	}

	Signal & operator += (const Slot<ParamsT...>& slot) {
		return attach(slot);
	}

	Signal & operator += (ReturnT(*func)(ParamsT...)) {
		return attach(func);
	}

	Signal & operator -= (const Slot<ParamsT...>& slot) {
		return detach(slot);
	}

	Signal & operator -= (ReturnT(*func)(ParamsT...)) {
		return detach(func);
	}

	void fire(ParamsT...params) const {
		for (int i = 0; i < _slotsCount; i++) {
			(*_connections[i])(params...);
		}
	}

	void operator ()(ParamsT...params)const {
		fire(params...);
	}
};

#endif
