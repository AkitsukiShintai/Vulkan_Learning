#pragma once
#include <memory>
#include <array>
#include <queue>
#include <iostream>
#include <assert.h>
#define  MAX_OBJECT_COUNT 5000

	template <class T,unsigned int size>
	class ObjectPool
	{
	public:
		friend class VulkanApplication;
		ObjectPool() :lastIndex(0) {
			
		}
		T* Get() {
			if (mEmptyQueue.empty())
			{
				T* newObj = &mPool[lastIndex];
				lastGetIndex = lastIndex;
				++lastIndex;
				assert(lastIndex < size);				
				return newObj;
			}
			else
			{
				T* oldObj = &mPool[mEmptyQueue.front()];
				lastIndex = mEmptyQueue.front();
				mEmptyQueue.pop();
				return oldObj;
			}
		}

		void Recycle(size_t index) {
			mEmptyQueue.push(index);
			mPool[index] = T();
		}

		size_t GetIndex() {		
			return lastGetIndex;
		}

		~ObjectPool() {

		}
	private:
		std::array<T, size> mPool;
		std::queue<size_t> mEmptyQueue;
		size_t lastIndex;
		size_t lastGetIndex;
	};


	template <class T>
	class ObjectPointerPool
	{
	public:
		ObjectPointerPool() :lastIndex(0) {

		}
		T* Get() {
			if (mEmptyQueue.empty())
			{
				T* newObj = mPool[lastIndex];
				++lastIndex;
				assert(lastIndex < 5000);
				return newObj;
			}
			else
			{
				T* oldObj = mPool[mEmptyQueue.front()];
				mEmptyQueue.pop();
				return oldObj;
			}
		}

		void Recycle(size_t index) {
			mEmptyQueue.push(index);
		}
		template <class B>
		void Init(std::array<B, MAX_OBJECT_COUNT>& pool) {
			for (size_t i = 0; i< MAX_OBJECT_COUNT ;++i)
			{
				mPool[i] = static_cast<T*>(&pool[i]);
			}
			
		}

		~ObjectPointerPool() {

		}
	private:
		std::array<T*, MAX_OBJECT_COUNT> mPool;
		std::queue<size_t> mEmptyQueue;
		size_t lastIndex;
	};
